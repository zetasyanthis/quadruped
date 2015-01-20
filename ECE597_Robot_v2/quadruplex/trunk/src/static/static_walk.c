#include <glib.h>
#include <unistd.h>
#include <math.h>

#include <rx28/rs485.h>
#include <rx28/rx28.h>

#include <static/static_motion.h>
#include <static/static_paths.h>

gint
main(gint argc, gchar **argv)
{
	if (rs485_open("/dev/ttyS1") < 0)
		g_error("couldn't open serial port.");

	g_print("Checking for servos on RS485 bus\n");
	gint total_present = 0; for (gint i = 0; i < 12; i++) {
		gint present = rx28_ping(i, NULL);
		g_print("id: %d, present: %d\n", i, present);
		if (present) total_present++;
	}
	if (total_present != 12) {
		g_error("%s missing.", total_present < 11 ? "servos are" : "a servo is");
	}

	g_print("Standing\n");
	static_playback_motion_array(stand);

	for (guint i = 0; i < 1; i++) {
		g_print("Walking\n");
		static_playback_motion_array(walk);
		static_playback_motion_array(walk);
		static_playback_motion_array(walk);
		static_playback_motion_array(walk);
		static_playback_motion_array(walk);
		static_playback_motion_array(walk);
		static_playback_motion_array(walk);
		//static_playback_motion_array(turnRight1);
		//static_playback_motion_array(turnRight1);
		//static_playback_motion_array(turnRight1);
		//static_playback_motion_array(turnRight1);
		//static_playback_motion_array(turnRight1);
	}

	g_print("Sitting\n");
	static_playback_motion_array(sit);

	rs485_close();

	return 0;
}

