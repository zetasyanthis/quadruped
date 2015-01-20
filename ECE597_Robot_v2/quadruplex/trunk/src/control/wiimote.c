#include <unistd.h>
#include <stdio.h>
#include <glib.h>
#include <gio/gio.h>
#include <cwiid.h>

#include "wiimote.h"

#define WIIMOTEDC_HOST "localhost"
#define WIIMOTEDC_PORT 2949

#define BUFFER_SIZE 1024

#define XOR(a, b) !!(a) ^ !!(b)

/// Locks access to the static variables in which the Wiimote's
/// state is stored.
static GStaticMutex data_mutex = G_STATIC_MUTEX_INIT;

/// Status global indicating the daemon's connection state.  Unknown
/// indicates that we are not connected to the daemon.
static enum wiimote_conn_status  wm_connected = WIIMOTE_CONN_NONE;
/// Status global containing the last button status message seen.
static struct cwiid_btn_mesg     wm_buttons = {0};
/// Status global containing the last Nunchuk status message seen.
static struct cwiid_nunchuk_mesg wm_nunchuk = {0};


/// Get Wiimote status data, which is copied to the output parameters given.
/// If not connected to the Wiimote daemon, return FALSE and do not update 
/// the output parameters.  Otherwise, return TRUE and update those output
/// parameters that are not NULL.
gboolean
wiimote_get_data(
	enum wiimote_conn_status *connected,
	struct cwiid_btn_mesg *buttons,
	struct cwiid_nunchuk_mesg *nunchuk
) {
	g_static_mutex_lock(&data_mutex);
	gboolean ret = (wm_connected == WIIMOTE_CONN_WIIMOTE);
	if (ret) {
		if (connected != NULL) {
			*connected = wm_connected;
		}
		if (buttons != NULL && wm_buttons.type != CWIID_MESG_UNKNOWN) {
			*buttons = wm_buttons;
		}
		if (nunchuk != NULL && wm_nunchuk.type != CWIID_MESG_UNKNOWN) {
			*nunchuk = wm_nunchuk;
		}
	}
	g_static_mutex_unlock(&data_mutex);
	return ret;
}


/// Parse a message line and update the global data structures.
static void
wiimote_process_message(gchar *line)
{
	g_static_mutex_lock(&data_mutex);
	// NOTE: Using sscanf to write directly to the globals is a bit unsafe, since
	// it could fail partway through, but the risk of this seems low enough that
	// it is not worth the effort of fixing.
	
	gchar mesg_type = line[0];
	gchar *mesg = line + 2;
	switch (mesg_type) {
	case 'c': // Wiimote connection
		{
			int connected = 0;
			int n = sscanf(mesg, "%d", &connected);
			if (n != 1) {
				g_warning("Error processing Wiimote c message \"%s\": %i matches", mesg, n);
			} else {
				wm_connected = connected ? WIIMOTE_CONN_WIIMOTE : WIIMOTE_CONN_DAEMON;
			}
		}
		break;
	case 'm': // Message
		{
			int data_type = CWIID_MESG_UNKNOWN;
			sscanf(mesg, "%d", &data_type);
			switch (data_type) {
				case CWIID_MESG_BTN:
					if (sscanf(mesg, "%u %" G_GUINT16_FORMAT,
							&wm_buttons.type, &(wm_buttons.buttons)) != 2) {
						g_warning("Error processing button message \"%s\"", mesg);
					}
					break;
				case CWIID_MESG_NUNCHUK:
					if (sscanf(mesg, "%u %hhu %hhu %hhu %hhu %hhu %hhu",
							&wm_nunchuk.type,
							&(wm_nunchuk.stick[0]), &(wm_nunchuk.stick[1]),
							&(wm_nunchuk.acc[0]), &(wm_nunchuk.acc[1]), &(wm_nunchuk.acc[2]),
							&(wm_nunchuk.buttons)) != 7) {
						g_warning("Error processing nunchuk message \"%s\"", mesg);
					}
					break;
				default:
					g_message("Unknown m type %d", data_type);
			}
		}
		break;
	default:
		g_warning("Discarding Wiimote message of type %c", mesg_type);
		break;
	}
	g_static_mutex_unlock(&data_mutex);
}

/// Read messages from the Wiimote daemon, updating the global static
/// data structures as messages arrive.
static gboolean
wiimote_daemon_read(GSocketConnection *conn, GError **error)
{
	g_assert(conn != NULL);

	gsize bytes_read = 0; // Number of bytes just read from the socket.
	gchar buffer[BUFFER_SIZE] = {0};
	gsize line_start = 0; // Start index of the line being processed.
	gsize buffer_index = 0; // Location in the buffer that we can write to
	GInputStream *input = g_io_stream_get_input_stream(G_IO_STREAM(conn));
	GError *comm_error = NULL;

	while (TRUE) {
		bytes_read = g_input_stream_read(input,
										&(buffer[buffer_index]),
										BUFFER_SIZE - buffer_index,
										NULL,
										&comm_error);
		if (comm_error != NULL) {
			g_error("Error reading: %s\n", comm_error->message);
			g_propagate_error(error, comm_error);
			return FALSE;
		} else {
			// Search for whole lines and process them
			gsize end_index = buffer_index + bytes_read;
			gsize i = buffer_index;
			while (i < end_index - 1) {
				if (buffer[i] == '\r' && buffer[i + 1] == '\n') { // End of message
					buffer[i] = '\0'; // Null terminate the string
					wiimote_process_message(&(buffer[line_start]));
					i += 2;
					line_start = i;
				} else {
					i++;
				}
			}
			if (line_start == 0 && end_index >= BUFFER_SIZE) {
				// The message is as big as our buffer... this shouldn't happen.
				g_error("Wiimote message larger than the buffer being discarded");
				//buffer_index = 0;
			} else if (end_index >= BUFFER_SIZE) {
				// We've reached the end of the buffer without hitting the end of the message.
				// Copy the prefix to the beginning of the buffer.
				for (buffer_index = 0; buffer_index < (end_index - line_start); buffer_index++) {
					buffer[buffer_index] = buffer[line_start + buffer_index];
				}
				line_start = 0;
			} else {
				buffer_index = end_index;
			}
		}
	}
	return TRUE;
}


/// Send a request to the Wiimote daemon that it indicate whether it
/// is presently connected to the Wiimote.  Return TRUE, or FALSE if
/// an error occurred (and also populate error).
static gboolean
wiimote_send_connected_command(GSocketConnection *conn, GError **error)
{
	g_assert(conn != NULL);
	gboolean ret = TRUE;
	
	gsize bytes_written = 0;
	GError *comm_error = NULL;
	
	GOutputStream *output = g_io_stream_get_output_stream(G_IO_STREAM(conn));
	g_output_stream_write_all(output, "c\r\n", 3, &bytes_written, NULL, &comm_error);
	g_assert(bytes_written == 3 || comm_error != NULL);
	
	if (comm_error != NULL) {
		g_propagate_error(error, comm_error);
		ret = FALSE;
	}
	
	g_assert(XOR(ret, *error != NULL));
	return ret;
}

/// Attempt to connect to the Wiimote daemon.  Give an error on failure and
/// return NULL, else return a pointer to a GSocketConnection object.
static GSocketConnection*
wiimote_daemon_connect(GSocketClient *client, GError **error)
{
	g_assert(client != NULL);

	GSocketConnection *conn = NULL;
	GError *conn_error = NULL;

	conn = g_socket_client_connect_to_host(client,
											WIIMOTEDC_HOST,
											WIIMOTEDC_PORT,
											NULL,
											&conn_error);
	if (conn_error != NULL) {
		g_propagate_error(error, conn_error);
		g_assert(conn == NULL);
	} else {
		g_static_mutex_lock(&data_mutex);
		wm_connected = WIIMOTE_CONN_DAEMON;
		// FIXME: Should we do anything with the globals here?
		g_static_mutex_unlock(&data_mutex);
	}
	
	g_assert(XOR(conn == NULL, *error == NULL));
	return conn;
}

/// Disconnect from the Wiimote daemon.  Return FALSE and set error
/// if there are problems, else return TRUE.  Set *conn to NULL in
/// any case.
static gboolean
wiimote_daemon_disconnect(GSocketConnection **conn, GError **error)
{
	g_assert(conn != NULL && *conn != NULL);
	gboolean ret = TRUE;
	
	g_static_mutex_lock(&data_mutex);
	wm_connected = WIIMOTE_CONN_NONE;
	wm_buttons.type = CWIID_MESG_UNKNOWN;
	wm_nunchuk.type = CWIID_MESG_UNKNOWN;
	g_static_mutex_unlock(&data_mutex);

	GError *conn_error = NULL;
	g_io_stream_close(G_IO_STREAM(*conn), NULL, &conn_error);
	if (conn_error != NULL) {
		ret = FALSE;
		g_propagate_error(error, conn_error);
	}

	g_object_unref(*conn);
	*conn = NULL;
	
	g_assert(XOR(*conn == NULL, *error == NULL));
	return ret;
}

/// Endlessly connect to the Wiimote daemon, communicate until disconnect,
/// and repeat.  On any sort of connection error, wait a second and
/// attempt to reconnect.
static gpointer
wiimote_thread(gpointer data)
{
	GError            *error  = NULL;
	GSocketClient     *client = g_socket_client_new();
	GSocketConnection *conn   = NULL;
	
	// Log the current error and clear it
	void log_and_clear(const gchar error_context[]) {
		g_message("Error %s: %s", error_context, error->message);
		g_clear_error(&error);
	}
	
	// Disconnect from the daemon and sleep for a second
	void disconnect() {
		if (!wiimote_daemon_disconnect(&conn, &error)) {
			log_and_clear("disconnecting from daemon");
		}
		sleep(1);
	}

	while (TRUE) { // TODO: Shutdown flag
		conn = wiimote_daemon_connect(client, &error);
		if (error != NULL) {
			log_and_clear("connecting to Wiimote daemon");
		} else {
			g_message("Connected to Wiimote daemon");
			if (!wiimote_send_connected_command(conn, &error)) {
				log_and_clear("writing to daemon");
				disconnect();
			}
		}
		while (conn != NULL) { // while connected
			if (!wiimote_daemon_read(conn, &error)) {
				log_and_clear("reading from daemon");
				disconnect();
			}
		}
	}
	
	g_assert(error == NULL);
	if (conn != NULL) {
		disconnect();
	}
	g_object_unref(client);
	return NULL;
}

/// Initialize the Wiimote daemon client, spawning a thread to update
/// the global variables.  If spawning the thread fails, call g_error().
void
wiimote_init(void)
{
	GError *thread_error = NULL;
	g_thread_create(wiimote_thread, NULL, FALSE, &thread_error);
	if (thread_error != NULL) {
		g_error("Unable to start the Wiimote thread: %s\n", thread_error->message);
	}
}

