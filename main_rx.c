#include <stdio.h>
#include <sched.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include <sys/shm.h>
#include <sys/ipc.h>

#include "srl_io.h"
#include "srl_buf.h"
#include "rt.h"

#define MAX_BUFF 10000000

void enable_raw_mode()
{
    struct termios term;
    tcgetattr(0, &term);
    term.c_lflag &= ~(ICANON | ECHO); // Disable echo as well
    tcsetattr(0, TCSANOW, &term);
}

void disable_raw_mode()
{
    struct termios term;
    tcgetattr(0, &term);
    term.c_lflag |= ICANON | ECHO;
    tcsetattr(0, TCSANOW, &term);
}

int kbhit()
{
    int byteswaiting;
    ioctl(0, FIONREAD, &byteswaiting);
    return byteswaiting > 0;
}

#define PORT0		0
#define PORT1		1
#define PORT2		2
#define PORT3		3

#pragma pack(0)

#define SIZE 263
unsigned long bytes_read;

unsigned char temp_arr[100];


static int sched_setattr(pid_t pid,
              const struct sched_attr *attr,
              unsigned int flags)
{
    return syscall(__NR_sched_setattr, pid, attr, flags);
}

static int sched_getattr (pid_t pid, struct sched_attr *attr,unsigned int size, unsigned int flags)
{
    return syscall (__NR_sched_getattr, pid, attr, size, flags);
}

unsigned char rx_data[SIZE];
unsigned char temp_buff[SIZE];
unsigned char large_buff[1000][SIZE+2];
unsigned char *large_ptr;

FILE* fp = 0; 

#define MAX_SAFE_STACK (8 *1024)
void stack_prefault(void)
{
        unsigned char dummy[MAX_SAFE_STACK];
        memset(dummy, 0, MAX_SAFE_STACK);
        return;
}

void main_rt(void *data)
{
	int i=0,j=0,retval=0;
	int prevbytes_count=0;
	unsigned int loop =0,loop_counter=0;
    int shm_id;
    key_t key=0;
    unsigned char* plant_log_addr,*shm_current_addr,*shm_current_addr_temp;
    int ret =0;

	struct timespec init_time,end_time,timediff;
	double init_time_ms;

	struct sched_param param;
	struct sched_attr attr;
    stack_prefault();
	fp = fopen("./v5_buf.log","wb");
    key = ftok("/home/bbr",'A');
    shm_id = shmget(key,MAX_BUFF,0666);
    if(shm_id<0)
    {
        printf("creating shm\n" );
        shm_id = shmget(key,MAX_BUFF,IPC_CREAT | 0666);
        if(shm_id<0)
        {
            perror("Error in shmget ");return 0;
        }
    }
    else
    {
        printf("shm id present, so reusing \n");
    }

    plant_log_addr = (unsigned char*)shmat(shm_id,NULL,0);    
    if(plant_log_addr < 0){
        perror("Error in attaching shm log1.Press a key to continue, CTRL+C to quit ");
        getchar();
    }
    else
    {
        shm_current_addr = (unsigned char*)plant_log_addr;
        printf("succesfully attached shm for log1.\n ");
    }
    //shmctl(shm_id,IPC_RMID,0);
    printf("initializing shm buffer to zeroes...\n");
    memset((void*)plant_log_addr,0,MAX_BUFF);

	large_ptr = large_buff;

	printf("Serial Communication .... \n");
        if ( mlockall(MCL_CURRENT | MCL_FUTURE ) == -1 )
        perror("mlockall:1");
	
	retval = SIO_open(PORT0, BAUD57600,DATABIT_8,STOPBIT_1,PAR_NO);
	if(retval < 0) {
	     printf("Failed to open read port:%d\n",retval);
		return 0;
	}
	else if(retval > 0) printf("Opened serial port succesfully\n");

	retval = sched_getattr(0, &attr, sizeof(struct sched_attr), 0);
    if(retval < 0)
       {perror("Error in get attr:");printf("%d\n",errno);return 0;}
    //attr.size = sizeof(attr);
    attr.sched_flags = 0;
    attr.sched_nice = 0;
    //attr.sched_priority = 0;
    //attr.sched_policy = SCHED_DEADLINE;
    attr.sched_policy = SCHED_FIFO;
    attr.sched_priority = 1;

    //attr.sched_runtime = 10*1000*10000;
    //attr.sched_deadline = 100*1000*1000;
    //attr.sched_period = 100*1000*1000;
    unsigned int flags=0;

	j = 0;
	printf("Enter a character\n");
	getchar();
    tcflush(stdin, TCIFLUSH); // Clear stdin to prevent characters appearing on prompt
    retval = sched_setattr(0,&attr,flags);
    if(retval<0)
    {
        perror("Error setting sched params:..... ");
        return 0;
    }

    //fcntl(mx_fd[PORT0],F_SETFL,fcntl(mx_fd[PORT0],F_GETFL,0)|O_NONBLOCK);
	retval =0;
	enable_raw_mode();
	while(!kbhit())
	{
		retval = SIO_read(PORT0,rx_data,SIZE); 
		if(retval<0)
		{
			perror("Error in read:");
		}
		else
        {
            bytes_read += retval;
			printf("bytes read :%d\n",retval);
            memcpy(shm_current_addr,rx_data,retval);
            /*
            for(i=0;i<retval;i++)
            {
                 printf("%u ",rx_data[i]);
            }     
            printf("\n");            
            */
            if(shm_current_addr < (plant_log_addr + MAX_BUFF - 263))
            {
                shm_current_addr += retval;
            }
            else
            {
                printf("shm near complete... exiting buffering loop\n");
                break;
            }
		}
    }
    disable_raw_mode();
    tcflush(0, TCIFLUSH); // Clear stdin to prevent characters appearing on prompt
	SIO_Close(PORT0);
    printf("bytes_read = %d\n", bytes_read);
    printf(" logging data from shm to file....\n");
    shm_current_addr_temp = (unsigned char*)plant_log_addr;
    //for(i=0;i<;i++)
    while(shm_current_addr_temp < shm_current_addr)
    {
        retval = fwrite(shm_current_addr_temp,1,1,fp);
        printf("%x", *shm_current_addr_temp);
        if(retval < 0)
        {
            perror("file write error....\n");
            break;
        }
        shm_current_addr_temp += 1;
    }
    fclose(fp);
    printf("\nclosing file\n");
    printf("analyzing data from shm....\n");
    shm_current_addr_temp = plant_log_addr;
    while(shm_current_addr_temp < shm_current_addr-263)
    {
        if(*shm_current_addr_temp == 0xfe)
        {
            printf("pktseq: %d .",shm_current_addr_temp[2]);
            printf("msgid %x .\n",shm_current_addr_temp[5]);
            
            switch(shm_current_addr_temp[5])
            {
                case 30:
                    if(shm_current_addr_temp[5] == 30)
                    {
                        printf("ATTITUDE\n");
                    }
                    break;
                case 0:    
                    if(shm_current_addr_temp[5] == 0)
                    {
                        printf("HEARTBEAT\n");
                    }
                    break;
                case 33:    
                    if(shm_current_addr_temp[5] == 33)
                    {
                        printf("GLOBAL_POS_INT\n");
                    }
                    break;
                default:
                    printf("msgid %x .\n",shm_current_addr_temp[5]);
            }
        }
        shm_current_addr_temp += 1;
    }

    //printf("releasing shm\n");  
    //shmdt(plant_log_addr);
	return 0;
}


int main()
{
    pthread_t thread;
    //printf("main thread [%ld]\n", gettid());
    pthread_create(&thread, NULL, main_rt, NULL);    
    pthread_join(thread, NULL);
    //printf("main dies [%ld]\n", gettid());
    return 0;
}

