#include <unistd.h>
#include <math.h>
#include <glib.h>
#include <cwiid.h>

#include "wiimote.h"

int main(int argc, char **argv)
{
	g_thread_init(NULL);
	g_type_init();
	wiimote_init();
	
	
	enum wiimote_conn_status  connected = WIIMOTE_CONN_NONE,
	                           last_connected = WIIMOTE_CONN_WIIMOTE;
	struct cwiid_btn_mesg     buttons = {0};
	struct cwiid_nunchuk_mesg nunchuk = {0};

	while (TRUE) {
		wiimote_get_data(&connected, &buttons, &nunchuk);
		if (connected == WIIMOTE_CONN_NONE && connected != last_connected) {
			g_print("Not connected to the daemon\n");
		} else if (connected == WIIMOTE_CONN_DAEMON && connected != last_connected) {
			g_print("Not connected to the Wiimote\n");
		} else if (connected == WIIMOTE_CONN_WIIMOTE) {
			g_print("Connected\n");
			if (buttons.type == CWIID_MESG_BTN) {
				g_print("  buttons %4d\n", buttons.buttons);
			}
			if (nunchuk.type == CWIID_MESG_NUNCHUK) {
				g_print("  joystick %3d %3d\n", nunchuk.stick[0], nunchuk.stick[1]);

				float x = CLAMP((nunchuk.stick[0] - 122) / 100.0, -1.0, 1.0);
				float y = CLAMP((nunchuk.stick[1] - 133) / 100.0, -1.0, 1.0);
				if (x < 0.1 && x > -0.1) x = 0;
				if (y < 0.1 && y > -0.1) y = 0;

				float angle = atan2f(x, y);
				float distance = sqrtf(x*x + y*y);
				distance = CLAMP(distance, 0, 1.0);
				if (angle > G_PI_2 || angle < -G_PI_2) distance = -distance;

				g_print("  x %+2.3f y %+2.3f\n", x, y);
				g_print("  angle %+2.3f distance %+2.3f\n", angle, distance);
			}
		}
		usleep(100000);
		last_connected = connected;
	}
	
	return 0;
}

