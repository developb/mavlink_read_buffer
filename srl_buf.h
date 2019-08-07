#ifndef SRL_BUF_H
#define SRL_BUF_H


//#include "srl_log_common.h"

/* Assumed maximum number of complete frames that can be fit in buffer, assuming frame size < buffer size */
#define NUM_FRAMES_BUF 100

/*
        Reserved, unused.
        In case we are reading n number of frames instead of random number of bytes every cycle NUM_FRAMES_BUF can be greater than
        or equal to NUM_FRAMES_READ, unless we are ready to ignore any received bytes.
*/
#define NUM_FRAMES_READ


/* 
	As needed based on tx cycle speed and rx buffering control parameters, our rx cycle execution time 

	Parameters:
	Tx cycle period (rate of arrival of new frames to rx fifo buffer, each time "ONE" new frame is added to rx fifo buffer )
	RX_CYCLE_PERIOD (cycle execution period, varies based on machine to machine) 

	Rx fifo buffer size
	RS422_FRAME_SIZE
	NUM_FRAMES_READ / or numbytes read at once at receiver end each rx cycle (?~ frame_size/fifo_size)
	NUM_FRAMES_BUF (>= NUM_FRAMES_READ. ie (frame_size/fifo_size)*2, like double buffering ))


	// This comment is not complete, 

	As sixdof frequency is anyways higher than that of logger, and we can log at least one cycle complete data every cycle, we can modify "RS422_FRAME_SIZE" and "rxbytes" in read(x,x,rxbytes) when we increase framesize. If sixdof cycle period is modified, You may need to modify RX_CYCLE_PERIOD accordingly.




*/
#define RX_CYCLE_PERIOD 10000000

#endif // SRL_BUF_H
