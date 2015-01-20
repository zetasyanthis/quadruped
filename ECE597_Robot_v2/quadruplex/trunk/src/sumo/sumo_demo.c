#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <glib.h>
#include <cwiid.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sumo/omap3530-pwm.h>
#include <sensors/i2c_bus.h>
#include <sensors/srf08.h>


gboolean
is_user_button_pressed()
{
    int fd = open("/sys/class/gpio/gpio7/value", O_RDONLY);
    if (fd < 0) {
        g_printerr("Error opening GPIO.  Have you run the init script?\n");
        return FALSE;
    }
    gchar buffer[2] = {0};
    if (read(fd, buffer, 1) < 1) {
        g_printerr("error reading: got \"%s\"\n", buffer);
    }
    close(fd);
    return buffer[0] == '1';
}


void
set_gpio(int gpio_num, gboolean value)
{
    gchar *fn = g_strdup_printf("/sys/class/gpio/gpio%d/value", gpio_num);
    int fd = open(fn, O_WRONLY);
    if (fd < 0) {
        g_printerr("Error opening GPIO %d.  Have you run the init script? %s\n", gpio_num, g_strerror(errno));
    } else {
        gchar *buffer = value ? "1\n" : "0\n";
        if (write(fd, buffer, 2) != 2) {
            g_printerr("Error writing to GPIO %d: %s\n", gpio_num, g_strerror(errno));
        }
        close(fd);
    }
    g_free(fn);
}


int
main(int argc, char **argv)
{
    g_thread_init(NULL);

    // TODO: Add a command-line argument to configure this
    gint strafe_counter_range = 200;

    I2CBus *bus = i2c_bus_open("/dev/i2c-2");
    SRF08 *sonar = srf08_new(bus, -1, -1, -1);
    i2c_bus_start_thread(bus);

    int mem_fd = pwm_open_devmem();
    if (mem_fd == -1) {
        g_error("Unable to open /dev/mem: %s", g_strerror(errno));
    }
    pwm_config_clock(mem_fd, TRUE, TRUE);

    guint8 *left_timer = pwm_mmap_instance(mem_fd, 10);
    guint8 *right_timer = pwm_mmap_instance(mem_fd, 11);
    g_assert(left_timer != MAP_FAILED);
    g_assert(right_timer != MAP_FAILED);
    guint32 resolution = pwm_calc_resolution(20000, PWM_FREQUENCY_13MHZ); 

    pwm_config_timer(left_timer, resolution, 0.0);
    pwm_config_timer(right_timer, resolution, 0.0);

    void set_speeds(float lspeed, float rspeed) {
        float ldc = ABS(lspeed);
        float rdc = ABS(rspeed);
        set_gpio(158, lspeed > 0.0);
        set_gpio(143, rspeed < 0.0);
        pwm_config_timer(left_timer, resolution, ldc);
        pwm_config_timer(right_timer, resolution, rdc);
    }

    gint strafe_counter = strafe_counter_range;
    void do_movement() {

        guint16 echoes[SRF08_NUM_ECHOES];
        srf08_get_data(sonar, NULL, echoes);

        if (echoes[0] < 30) {
            set_speeds(1.0, 1.0); // ATTACK!!!
            //g_print("!");
        } else {
            if (strafe_counter < 0) { // Strafe left
                set_speeds(0.5, -0.5);
                //g_print("<");
            } else { // Strafe right
                set_speeds(-0.5, 0.5);
                //g_print(">");
            }
            if (strafe_counter-- < -strafe_counter_range) {
                strafe_counter = -strafe_counter;
            }
        }
    }

    g_print("Waiting...\n");
    while (!is_user_button_pressed()) {
        usleep(50000);
    }

    g_print("User button pressed: starting\n");
    for (int i = 5; i > 0; i--) {
        g_print("%d... ", i);
        sleep(1);
    }
    g_print("\n");

    while (!is_user_button_pressed()) {
        do_movement();
        usleep(10000);
    }

    g_print("\nUser button pressed: stopping\n");
    pwm_config_timer(left_timer, resolution, 0.0);
    pwm_config_timer(right_timer, resolution, 0.0);
    
    pwm_close_devmem(mem_fd);
    return 0;
}

