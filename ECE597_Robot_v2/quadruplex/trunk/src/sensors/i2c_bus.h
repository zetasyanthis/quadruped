/// Maximum number of drivers on a bus at a time
#define I2C_DRIVERS_MAX 10

#define I2C_ADDR_MIN 0
#define I2C_ADDR_MAX 127


// Forward declaration.
typedef struct _I2CBus I2CBus;

/// Prototype for driver callback functions. They are passed the bus and an opaque pointer.
typedef void (*i2c_driver_callback) (I2CBus *bus, gpointer user_data);


struct _I2CBus {
    gchar *dev_filename;
    int dev_fd;
    // Pointers to functions that do I/O on the I2C bus
    i2c_driver_callback drivers[I2C_DRIVERS_MAX];
    void **user_data[I2C_DRIVERS_MAX];
    size_t num_drivers;
};


I2CBus *i2c_bus_open(gchar *filename);
gboolean i2c_bus_add_driver(I2CBus *bus, i2c_driver_callback callback, void *user_data);
void i2c_bus_close(I2CBus *bus);
void i2c_bus_start_thread(I2CBus *bus);
int i2c_bus_read_buffer(I2CBus *bus, int i2c_addr, int reg, gssize len, guint8 buffer[len], gboolean report_errors);
int i2c_bus_read_byte(I2CBus *bus, int i2c_addr, int reg, gboolean report_errors);
gboolean i2c_bus_write_buffer(I2CBus *bus, int i2c_addr, int reg, gssize len, guint8 buffer[len], gboolean report_errors);
gboolean i2c_bus_write_byte(I2CBus *bus, int i2c_addr, int reg, guint8 byte, gboolean report_errors);
