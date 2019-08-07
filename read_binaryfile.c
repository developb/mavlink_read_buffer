#include <stdio.h>
#include <unistd.h>

unsigned char c[10000000];

int main()
{
	int i=0;
	FILE* fp = fopen("v5_buf.log","rb");
	if(fp <=0) { perror("error"); return 0; }

	while(!feof(fp))
	{
		fread(&c[i],1,1,fp);
		printf("%x",c[i]);
		i++;
	}
	fclose(fp);
	return 0;
}
