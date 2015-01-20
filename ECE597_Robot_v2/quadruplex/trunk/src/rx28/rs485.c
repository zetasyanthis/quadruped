#include <stdlib.h>
#include <glib.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <sys/ioctl.h>
#include "rs485.h"

static gint devfd = -1;

gint
rs485_open(const gchar *dev)
{
   if ((devfd = open(dev, O_RDWR | O_NOCTTY)) == -1) {
      g_warning("Unable to open serial port: %s", g_strerror(errno));
      return -1;
   }

   struct termios tio;
   // This call probably isn't needed, since I think cfmakeraw
   // overwrites everything anyway and I don't bother restoring the
   // previous settings when we quit
   if (tcgetattr(devfd, &tio) == -1) return -1;

   cfmakeraw(&tio);
   cfsetspeed(&tio, B500000);

   tcflush(devfd, TCIOFLUSH);
   if (tcsetattr(devfd, TCSANOW, &tio) == -1)
      return -1;

#ifdef BEAGLEBOARD
   struct serial_rs485_settings settings = {
      .flags = SER_RS485_MODE_RTS|SER_RS485_RTS_TX_LOW,
      .delay_before_send = 50*1000,
      .delay_after_send  = 60*1000, 
   };
   if (ioctl(devfd, TIOCSRS485, &settings) != 0)
      g_warning("Could not setup half-duplex RS485: %s", g_strerror(errno));
#endif

   return 0;
}

gint
rs485_close(void)
{
   if (devfd == -1) return -1;
   close(devfd);
   return 0;
}


gint
rs485_send(const guint8 *buf, gsize len, gsize *bytes_written)
{
   if (devfd == -1) return -1;
   if (!buf) return -1;

   gsize count = write(devfd, buf, len);

   if (bytes_written) *bytes_written = count;
   if (count == -1 || count != len) return -1;

   return 0;
}

gint
rs485_recv(guint8 *buf, gsize len, gsize *bytes_read)
{
   gsize count;
   struct pollfd pfd = {
      .fd      = devfd,
      .events  = POLLIN,
      .revents = POLLIN
   };

   if (devfd == -1) return -1;
   if (!buf) return -1;

   if (poll(&pfd, 1, RX_TIMEOUT) <= 0)
      return -1;

   count = read(devfd, buf, len);
   if (bytes_read) *bytes_read = count;
   
   if (count == -1 || count != len)
      return -1;

   return 0;
}

gint
rs485_getc(void)
{
   guint8 tmp;

   if (rs485_recv(&tmp, 1, NULL) == -1) return -1;

   return tmp;
}
