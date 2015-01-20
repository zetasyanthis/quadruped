#include <glib.h>
#include <gio/gio.h>
#include <cwiid.h>

#ifndef G_THREADS_ENABLED
#  error "GLib must be compiled with thread support"
#endif

/// Enum indicating whether the Wiimote daemon is connected to a Wiimote.
enum wiimote_conn_status {
  	WIIMOTE_CONN_NONE, /// Not connected to the daemon
	WIIMOTE_CONN_DAEMON, /// Just connected to the daemon
	WIIMOTE_CONN_WIIMOTE /// Connected to daemon, which is connected to a Wiimote
};

void wiimote_init(void);
gboolean wiimote_get_data(enum wiimote_conn_status *, struct cwiid_btn_mesg *, struct cwiid_nunchuk_mesg *);

