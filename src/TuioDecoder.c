#include "TuioDecoder.h"

#include <string.h>

void TuioFrameInit(TuioFrame* frame)
{
    memset(frame, 0, sizeof(TuioFrame));
}

int TuioDecodePacket(const uint8_t* packet,
                     uint32_t length,
                     TuioFrame* frame)
{
    (void)packet;
    (void)length;

    TuioFrameInit(frame);

    return 0;
}

void TuioFrameToString(const TuioFrame* frame,
                       char* buffer,
                       int bufferSize)
{
    (void)frame;

    if (bufferSize > 0)
        buffer[0] = 0;
}
