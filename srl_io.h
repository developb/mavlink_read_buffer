#ifndef _SERIALIO_H
#define _SERIALIO_H

extern int mx_fd[5];

typedef enum {BAUD110,BAUD300,BAUD1200,BAUD2400,BAUD4800,BAUD9600,BAUD19200,BAUD38400,BAUD57600,BAUD115200,BAUD230400,BAUD460800,BAUD921600 } SIO_Baud;
typedef enum {DATABIT_5,DATABIT_6,DATABIT_7,DATABIT_8 } SIO_DataBits;
typedef enum {STOPBIT_1,STOPBIT_1p5,STOPBIT_2 } SIO_StopBits;
typedef enum {PAR_NO,PAR_ODD,PAR_EVEN } SIO_parity;

int SIO_open(unsigned short arg_ChannelNum, SIO_Baud argBaud,SIO_DataBits argDataBits,SIO_StopBits argStopBits,SIO_parity arg_parity);
int SIO_write(unsigned short arg_ChannelNum, unsigned char arg_buf[],unsigned short arg_dataLen);
int SIO_read(unsigned short arg_ChannelNum, unsigned char arg_buf[],unsigned short arg_dataLen);
int SIO_Close(unsigned short arg_ChannelNum);

#endif