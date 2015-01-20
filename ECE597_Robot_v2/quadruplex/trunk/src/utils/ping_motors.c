#include <glib.h>
#include <stdlib.h>
#include <signal.h>

#include <helper.h>
#include <rx28/rs485.h>
#include <rx28/rx28.h>

enum {
	VERB_SILENT = 0,
	VERB_QUIET = 1,
	VERB_LOUD = 2
};

gint
main(gint argc, gchar **argv)
{
	gboolean repeat = 0;
	gboolean verbosity = 2;

	GError *option_error = NULL;
	GOptionEntry option_entries[] = {
		{"repeat", 'r', 0, G_OPTION_ARG_NONE, &repeat,
		 "Ping repeatedly", NULL},
		{"verbosity", 'v', 0, G_OPTION_ARG_INT, &verbosity,
		 "Verbosity level - 0 silent, 1 quiet, 2 default", NULL},
		{NULL},
	};
	GOptionContext *option_context = g_option_context_new(" - ping the motors");
	g_option_context_add_main_entries(option_context, option_entries, NULL);
	if (!g_option_context_parse(option_context, &argc, &argv, &option_error)) {
		g_printerr("Error: %s\n", option_error->message);
		exit(2);
	}

	if (rs485_open("/dev/ttyS1") < 0)
		g_error("couldn't open serial port");

	// Signal handler to end the repeat
	void end_repeat(int arg)
	{
		repeat = 0;
	}

	if (repeat) {
		signal(SIGINT, end_repeat);
	}

	if (verbosity >= VERB_LOUD) {
		g_print("Checking for servos on RS485 bus\n");
	}

	gint successes = 0;
	gint failures = 0;
	do {
		gint total_present = 0;
		for_each_motor (i) {
			gint present = rx28_ping(i, NULL);
			if (verbosity == VERB_QUIET) {
				g_print(present ? "." : "!");
			} else if (verbosity >= VERB_LOUD) {
				g_print("id: %2d, present: %d\n", i, present);
			}
			if (present) {
				successes++;
				total_present++;
			} else {
				failures++;
			}
		}
	} while (repeat);

	if (verbosity == VERB_QUIET) g_print("\n");
	if (verbosity >= VERB_QUIET) {
		gint total = successes + failures;
		g_print("%d pings made, %d successful and %d failures\n", total, successes, failures);
		g_print("%.1f%% success rate\n", 100.0 * (float)successes / (float)total);
	}

	rs485_close();
	return !failures;
}

