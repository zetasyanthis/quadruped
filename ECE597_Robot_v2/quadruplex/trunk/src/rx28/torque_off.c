#include <glib.h>
#include <stdlib.h>
#include <unistd.h>
#include <helper.h>
#include <rx28/rs485.h>
#include <rx28/rx28.h>
#include <rx28/rx28_simple.h>

gint
main(gint argc, gchar **argv)
{
    if (rs485_open("/dev/ttyS1") < 0)
       g_error("couldn't open serial port.\n");

    gboolean selected_motors[12] = {0};

    if (argc == 1) {
        for_each_motor (motor_id) {
            selected_motors[motor_id] = TRUE;
        }
    } else {
        for (gint i = 1; i < argc; i++) {
            int motor_id = atoi(argv[i]);
            if (motor_id >= 0 && motor_id <= 12) {
                selected_motors[motor_id] = TRUE;
            } else {
                g_printerr("Bad motor ID %s", argv[i]);
            }
        }
    }
 
    for_each_motor (motor_id) {
        if (selected_motors[motor_id]) {
            rx28_set_torque_enable(motor_id, 0);
        }
    }
 
    rs485_close();
 
    return 0;
}





