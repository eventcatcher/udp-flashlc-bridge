/** Author: Georg Kaindl **/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#include "UDPListener.h"
#include "TFFlashLCSHMEM.h"
#include "TuioDecoder.h"

#define  DEFAULT_UDP_PORT        (3333)
#define  DEFAULT_LC_CONN_NAME    ((char*)"_OscDataStream")
#define  DEFAULT_LC_METH_NAME    ((char*)"receiveOscData")

typedef enum logLevel_t {
   LogLevelNone      = 0,
   LogLevelErrors    = 1,
   LogLevelInfo      = 2,
   LogLevelVerbose   = 3
} logLevel_t;

static logLevel_t logLevel = LogLevelInfo;
static TFLCSLocalConnection_t* lcConnection;

static void LogMessage(logLevel_t level, const char* format, ...)
{
   if (level > logLevel)
      return;

   static const char* levelNames[] = {
      "",
      "ERROR",
      "INFO",
      "DEBUG"
   };

   fprintf(stderr, "[%s] ", levelNames[level]);

   va_list args;
   va_start(args, format);
   vfprintf(stderr, format, args);
   va_end(args);

   fprintf(stderr, "\n");
}

static void DumpPacket(const uint8_t* packet, uint32_t length)
{
   uint32_t i;

   for (i = 0; i < length; i += 16) {

      char hex[16 * 3 + 1];
      char ascii[16 + 1];

      memset(hex, ' ', sizeof(hex));
      hex[sizeof(hex)-1] = 0;

      memset(ascii, ' ', sizeof(ascii));
      ascii[16] = 0;

      uint32_t j;
      for (j = 0; j < 16 && (i + j) < length; j++) {

         uint8_t b = packet[i + j];

         sprintf(&hex[j * 3], "%02X ", b);

         ascii[j] =
            (b >= 32 && b < 127)
            ? (char)b
            : '.';
      }

      LogMessage(LogLevelVerbose,
                 "%04X  %-48s %s",
                 i,
                 hex,
                 ascii);
   }
}

static void LogTuioFrame(const TuioFrame* frame)
{
   int i;

   LogMessage(LogLevelVerbose,
              "TUIO /tuio/2Dcur: frame=%d source=%s alive=%d touches=%d",
              frame->frame,
              frame->source[0] ? frame->source : "(none)",
              frame->aliveCount,
              frame->touchCount);

   if (frame->aliveCount > 0) {
      char aliveList[512];
      int offset = 0;

      aliveList[0] = 0;

      for (i = 0; i < frame->aliveCount; i++) {
         int written = snprintf(aliveList + offset,
                                sizeof(aliveList) - (size_t)offset,
                                "%s%d",
                                (i == 0) ? "" : ", ",
                                frame->aliveIds[i]);

         if (written < 0)
            break;

         if ((size_t)written >= sizeof(aliveList) - (size_t)offset) {
            offset = (int)sizeof(aliveList) - 1;
            break;
         }

         offset += written;
      }

      LogMessage(LogLevelVerbose, "  alive session ids: %s", aliveList);
   }

   for (i = 0; i < frame->touchCount; i++) {
      const TuioTouch* touch = &frame->touches[i];

      LogMessage(LogLevelVerbose,
                 "  touch #%d: session=%d pos=(%.4f, %.4f) velocity=(%.4f, %.4f) motionAccel=%.4f",
                 i + 1,
                 touch->sessionId,
                 touch->x,
                 touch->y,
                 touch->dx,
                 touch->dy,
                 touch->motionAccel);
   }
}

void HandleUDPPacketReceived(const uint8_t* packet,
                             uint32_t packetLength,
                             const char* sourceAddress,
                             uint16_t sourcePort);

char* myStrdup(const char* src)
{
   char* rv = (char*)malloc(strlen(src)+1);
   strncpy(rv, src, strlen(src)+1);
   return rv;
}

void usageAndDie(const char* pName)
{
   fprintf(stderr,
      "Usage: %s [-p <udp listening port>] [-l <loglevel>] [-c LC name] [-m LC method name]\n", pName
   );
   
   exit(-1);
}

int main(int argc, char** argv)
{
   int c;
   uint16_t udpPort = DEFAULT_UDP_PORT;
   char* lcConnName = myStrdup(DEFAULT_LC_CONN_NAME);
   char* lcMethName = myStrdup(DEFAULT_LC_METH_NAME);
   
   UDPListener* udpListener;
   
   opterr = 0;
   
   while (-1 != (c = getopt(argc, argv, "p:l:c:m:h"))) {
      switch (c) {
         case 'p': {
               int p = atoi(optarg);
               if (p < 0 || p > 65535)
                  usageAndDie(argv[0]);
               udpPort = p;
               break;
            }
         case 'l':
            logLevel = atoi(optarg);
            break;
         case 'c':
            lcConnName = myStrdup(optarg);
            break;
         case 'm':
            lcMethName = myStrdup(optarg);
            break;
         case 'h':
         default:
            usageAndDie(argv[0]);
            break;
      }
   }
   
   if (optind < argc)
      usageAndDie(argv[0]);
   
   if (logLevel >= LogLevelVerbose) {
      printf(
         "UDP listening port: %d\n"
         "Log Level: %d\n",
         udpPort, logLevel
      );
   }
   
#if defined(WINDOWS)
   WSADATA wsaData;
   WSAStartup(MAKEWORD(2, 0), &wsaData);
#endif
   
   // set up the UDP listener
   udpListener = UDPListenerCreateWithPort(udpPort);
   
   if (NULL == udpListener) {
      if (logLevel >= LogLevelErrors)
         LogMessage(LogLevelErrors, "UDPListener creation: out of memory.");
      exit(-1);
   }
   
   if (!UDPListenerIsValid(udpListener)) {
      if (logLevel >= LogLevelErrors)
         LogMessage(LogLevelErrors, "UDPListener: %s",
            UDPListenerStringForError(udpListener->lastError));
      exit(-1);
   }
   
   if (logLevel >= LogLevelInfo) {
      LogMessage(LogLevelInfo,"Listening for UDP at port %d.", udpListener->port);
   }
   
   UDPListenerSetPacketReceiptCallback(udpListener, &HandleUDPPacketReceived);
   
   // set up the flash localconnection sender
   lcConnection = TFLCSConnect(lcConnName,
                               lcMethName,
                               NULL,
                               NULL);
   
   if (logLevel >= LogLevelInfo) {
      LogMessage(LogLevelInfo,"Serving to Flash via LocalConnection at %s:%s", lcConnName, lcMethName);
   }
   
   free(lcConnName);
   free(lcMethName);
   
   while(1)
      UDPListenerWaitForPacket(udpListener);
   
   return 1;
}

void HandleUDPPacketReceived(const uint8_t* packet,
                             uint32_t packetLength,
                             const char* sourceAddress,
                             uint16_t sourcePort)
{
   if (logLevel >= LogLevelVerbose) {
      TuioFrame frame;

      LogMessage(LogLevelVerbose, "Received %u bytes from %s:%u", packetLength, sourceAddress, sourcePort);

      if (TuioDecodePacket(packet, packetLength, &frame)) {
         LogTuioFrame(&frame);
      } else {
         LogMessage(LogLevelVerbose, "Packet is not recognized as TUIO; dumping raw bytes.");
         DumpPacket(packet, packetLength);
      }
   }
   
   // Flash 10 seems not to be able to keep up with very fast rates sometimes?
   // at my trivial Flash test-app sometimes throws an exception (but does not choke on it)
   // when an app like MSARemote sends 120+ packets per second.
   // below 100 packets/second seem to work fine in any case, though.
   // maybe it's good enough to catch the exception in Flash and just drop it?
   /*static int t = 0;
   if (t++ % 3)
      return;*/
      
   if (TFLCSConnectionHasConnectedClient(lcConnection)) {
      TFLCSSendByteArray(lcConnection, (char*)packet, packetLength);
      LogMessage(LogLevelVerbose, "Forwarded %u bytes to Flash.", packetLength);
   }
}
