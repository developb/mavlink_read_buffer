#include <stdio.h>     /* Standard input/output definitions */
#include <unistd.h>    /* UNIX standard function definitions */
#include <fcntl.h>     /* File control definitions */
#include <termios.h>   /* POSIX terminal control definitions */
#include <string.h>    /* String function definitions */
#include <errno.h>     /* Error number definitions */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>


#include "srl_io.h"
#define U8 unsigned char
#define S8 char
int mx_fd[5];
int k=0;

  char devName[20];
int SIO_open(unsigned short arg_ChannelNum, SIO_Baud argBaud,SIO_DataBits argDataBits,SIO_StopBits argStopBits,SIO_parity arg_parity)
{
  struct termios options;
	int ret=0;
     
  if (arg_ChannelNum >3 )
  {
    //return -1;
  }
  //sprintf(devName,"/dev/ttyAP0",arg_ChannelNum);
  sprintf(devName,"/dev/ttyUSB0"); //,arg_ChannelNum);
  //sprintf(devName,"/dev/ttyS4");
  printf("Opening port %s\n",devName);

  // if (mx_fd[arg_ChannelNum])
  // close(mx_fd[arg_ChannelNum]);

  mx_fd[arg_ChannelNum] = open(devName,O_RDWR);
	if(mx_fd[arg_ChannelNum] < 0)
	{
		perror("unable to open port. ");
		return 0;
	}	

  ret = tcflush(mx_fd[arg_ChannelNum], TCIOFLUSH);
  if(ret !=0)
{
	perror("Error flushing file descriptor\n");
	return 0;
	}

	//ioctl(mx_fd[arg_ChannelNum],TIOCSFIFOTRIG,&rx_trig);
	//printf("%X is rx_trig\n",rx_trig);
  if (mx_fd[arg_ChannelNum] <0)
  {
    perror("open_port: Unable to open ");   // Could not open the port.
  }
  else	{
   	  //fcntl(mx_fd[arg_ChannelNum],F_SETFL,fcntl(mx_fd[arg_ChannelNum],F_GETFL,0)|O_NONBLOCK);
    //fcntl(mx_fd[arg_ChannelNum],F_SETFL,0);
    //fcntl(mx_fd[arg_ChannelNum],F_SETFL,O_NONBLOCK);
 } 
  tcgetattr(mx_fd[arg_ChannelNum], &options);  /* Get the current options for the port...*/
  tcflush(mx_fd[arg_ChannelNum], TCIOFLUSH); 
//SIO_parity
  switch(arg_parity)
  {      
    case PAR_NO  : options.c_cflag &=~(PARENB);
		   break;
    case PAR_ODD : options.c_cflag |=(PARODD | PARENB);
		   break;
    case PAR_EVEN: options.c_cflag |= PARENB;
		   options.c_cflag &= ~(PARODD);
		   break;
    default	 : options.c_cflag |= (PARODD | PARENB);
		   break;
  }
  //SIO_DataBits
  options.c_cflag &= ~(CSIZE );   
  
  switch(argDataBits)
  {
    case DATABIT_5 : options.c_cflag |= CS5;
		     break;
    case DATABIT_6 : options.c_cflag |= CS6;
		     break;
    case DATABIT_7 : options.c_cflag |= CS7;
		     break;
    case DATABIT_8 : options.c_cflag |= CS8;
		     break;
    default:         options.c_cflag |= CS8;
		     break;
  }
  //SIO_StopBits
  switch(argStopBits)
  {
    case STOPBIT_1 : options.c_cflag &= ~CSTOPB;
		     break;
    case STOPBIT_2 : options.c_cflag |= CSTOPB;
		     break;
    default:  	     options.c_cflag &= ~CSTOPB;
		     break;
  }
  //SIO_Baud 
  switch(argBaud)
  {
	printf("%d is baud\n",argBaud);
    case BAUD110 : cfsetispeed(&options,B110);
		   cfsetospeed(&options,B110);
		  break;
    case BAUD300 : cfsetispeed(&options,B300);
		   cfsetospeed(&options,B300);
		  break;
    case BAUD1200 : cfsetispeed(&options,B1200);
		    cfsetospeed(&options,B1200);
		  break;
    case BAUD2400 : cfsetispeed(&options,B2400);
		    cfsetospeed(&options,B2400);
		  break;
    case BAUD4800 : cfsetispeed(&options,B4800);
		    cfsetospeed(&options,B4800);
		  break;
    case BAUD9600 : cfsetispeed(&options,B9600);
		    cfsetospeed(&options,B9600);
		  break;
    case BAUD19200 : cfsetispeed(&options,B19200);
		     cfsetospeed(&options,B19200);
		  break;
    case BAUD38400 : cfsetispeed(&options,B38400);
		     cfsetospeed(&options,B38400);
		  break;
   
		
    case BAUD115200 : cfsetispeed(&options,B115200);
		      cfsetospeed(&options,B115200);
		
		  break;
    case BAUD230400 : cfsetispeed(&options,B230400);
		      cfsetospeed(&options,B230400);
		  break;
	
    case BAUD460800 : cfsetispeed(&options,B460800);
		      cfsetospeed(&options,B460800);
		  break;
		  
#if 0
    case BAUD921600 : cfsetispeed(&options,B2400);
		      cfsetospeed(&options,B2400);
		  break;
#endif
    case BAUD921600 : cfsetispeed(&options,B921600);
		      cfsetospeed(&options,B921600);
		  break;
     
    default      : cfsetispeed(&options,B9600);
		  break;
  }
  options.c_cflag |= (CLOCAL | CREAD); /* Enable the receiver and set local mode...  */
  options.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR |ICRNL | IXON);
  options.c_oflag &= ~OPOST;
  options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);

  tcsetattr(mx_fd[arg_ChannelNum], TCSANOW, &options);
  
  return (mx_fd[arg_ChannelNum]);  
}

int SIO_write(unsigned short arg_ChannelNum, unsigned char arg_buf[],unsigned short arg_dataLen)
{  
//printf("\nA") ;
return (write(mx_fd[arg_ChannelNum],arg_buf,arg_dataLen)); 

}

int SIO_read(unsigned short arg_ChannelNum, unsigned char arg_buf[],unsigned short arg_dataLen)
{
//unsigned int  value=0;

//ioctl(mx_fd[arg_ChannelNum],TIOCSERGETLSR, 0);
return (read(mx_fd[arg_ChannelNum],arg_buf,arg_dataLen)); 

/*U8 bytesRead=0;
  S8 retVal=0;
  U8 temp;
	printf("in read fun");  
  while(bytesRead<arg_dataLen)
  { 
    retVal=read(mx_fd[arg_ChannelNum],&temp ,1 ) ;
    if(retVal>0)
  	arg_buf[bytesRead++]=temp;
	//bytesRead++;       
  }
  return bytesRead; */ 
}

int SIO_Close(unsigned short arg_ChannelNum)
{  
  close(mx_fd[arg_ChannelNum]);
}
