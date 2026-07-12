#ifndef TUIO_DECODER_H
#define TUIO_DECODER_H

#include <stdint.h>

#define TUIO_MAX_TOUCHES 32

typedef struct
{
    int sessionId;

    float x;
    float y;

    float dx;
    float dy;

    float motionAccel;

} TuioTouch;

typedef struct
{
    char source[64];

    int frame;

    int aliveIds[TUIO_MAX_TOUCHES];
    int aliveCount;

    TuioTouch touches[TUIO_MAX_TOUCHES];
    int touchCount;

} TuioFrame;

void TuioFrameInit(TuioFrame* frame);

int TuioDecodePacket(const uint8_t* packet,
                     uint32_t length,
                     TuioFrame* frame);

void TuioFrameToString(const TuioFrame* frame,
                       char* buffer,
                       int bufferSize);

#endif
