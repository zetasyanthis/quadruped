
typedef enum {
   INPUT, OUTPUT
} pin_dir_t;

gint gpio_set(guint8 pin, gboolean value);
gint gpio_set_direction(guint8 pin, pin_dir_t dir);


