#include <glib.h>
#include <unistd.h>
#include "rs485.h"
#include "rx28.h"
#include "rx28_simple.h"

gint
main(gint argc, gchar **argv)
{
   if (rs485_open("/dev/ttyS1") < 0)
      g_error("couldn't open serial port.\n");

   for (gint i = 0; i < 12; i++) {
      g_print("id: %x, present: %d\n", i, rx28_ping(i, NULL));
   }

	return 0;

   guint16 cur_pos;

   for (gint i = 0; i < 12; i++) {
      cur_pos = rx28_get_present_position(i%12);
      g_print("id: %x, position: %u\n", i%12, cur_pos);
   }

   rs485_close();

   return 0;
}
