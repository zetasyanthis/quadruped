#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>
#include "gpio.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


static const gchar gpio_dir_str[] = "/sys/class/gpio";
static gint gpio_export(guint8 pin);
static FILE *gpio_open_subdir(guint8 pin, const gchar *subdir, const gchar *mode);
static gboolean gpio_is_exported(guint8 pin);

static gboolean
gpio_is_exported(guint8 pin)
{
   GDir *gpio_dir;
   gpio_dir = g_dir_open(gpio_dir_str, 0, NULL);
   if (!gpio_dir) return FALSE;

   gchar *dir_name;
   gboolean found_gpio_dir = FALSE;
 
   gchar *wanted_name = g_strdup_printf("gpio%d", pin);

   while ((dir_name = (gchar *)g_dir_read_name(gpio_dir))) {
      if (!g_ascii_strcasecmp(wanted_name, dir_name)) {
         found_gpio_dir = TRUE;
         break;
      }
   }

   g_dir_close(gpio_dir);
   g_free(wanted_name);

   return found_gpio_dir;
}
      


static FILE *
gpio_open_subdir(guint8 pin, const gchar *subdir, const gchar *mode)
{
   FILE *f;
   gchar *pin_dir_str = g_strdup_printf("gpio%d", pin);
   gchar *subdir_str = g_build_filename(gpio_dir_str, pin_dir_str, subdir, NULL);
   
   f = fopen(subdir_str, mode);
   g_free(pin_dir_str);
   g_free(subdir_str);
   return f;
}



static gint
gpio_export(guint8 pin)
{
   gchar *export_dir = g_build_filename(gpio_dir_str, "export", NULL);
   FILE *export_fd = fopen(export_dir, "w");

   g_free(export_dir);
   if (!export_fd) g_error("Could not open gpio export file");

   fprintf(export_fd, "%d", pin);

   fclose(export_fd);
   return 0;
}


gint
gpio_set_direction(guint8 pin, pin_dir_t dir)
{
   // a check should be added to see if it's already exported
   if (!gpio_is_exported(pin)) if (gpio_export(pin) < 0) return -1;

   FILE *direction_fd = NULL;

   direction_fd = gpio_open_subdir(pin, "direction", "w");
   if (!direction_fd) {
      g_warning("could not open direction file");
      return -1;
   }

   fprintf(direction_fd, (dir == OUTPUT) ? "out" : "in");

   fclose(direction_fd);
   return 0;
}


// This needs to be rewritten to cache the file handle instead of
// opening and closing it each time it's called.  maybe maintain a list
// of pinstructs containing pin number, a file handle, &c.
gint
gpio_set(guint8 pin, gboolean value)
{

   //int fd;
   //char on = '1', off = '0';
   //if (pin == 136)
   //   fd = open("/sys/class/gpio/gpio136", O_WRONLY);
   //if (pin == 137)
   //   fd = open("/sys/class/gpio/gpio137", O_WRONLY);

   //if (value)
   //   write(fd, &on, 1);
   //else
   //   write(fd, &off, 1);

   //close(fd);
   //return 0;

   FILE *value_fd = gpio_open_subdir(pin, "value", "w");
   if (!value_fd) {
      g_warning("could not open value file");
      return -1;
   }

   fprintf(value_fd, value ? "1" : "0");
   fclose(value_fd);
   //value_fd = gpio_open_subdir(pin, "value", "r+");
   //gint pin_state = 0;
   //do {
   //   fscanf(value_fd, "%d\n", &pin_state);
   //} while (value != (!!pin_state));

   //fclose(value_fd);
   return 0;
}
