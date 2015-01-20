
#define SRF08_DEFAULT_ADDRESS      (0xe0 >> 1)
#define SRF08_DEFAULT_ANALOG_GAIN  31
#define SRF08_DEFAULT_RANGE        255

#define SRF08_ANALOG_GAIN_MIN 0
#define SRF08_ANALOG_GAIN_MAX 31

#define SRF08_RANGE_MIN 0
#define SRF08_RANGE_MAX 255

#define SRF08_COMMAND_RANGE_IN   0x50
#define SRF08_COMMAND_RANGE_CM   0x51
#define SRF08_COMMAND_RANGE_USEC 0x52

#define SRF08_COMMAND_REG 0x00
#define SRF08_LIGHT_REG   0x01
#define SRF08_GAIN_REG    0x01
#define SRF08_RANGE_REG   0x02
#define SRF08_ECHO_REG    0x02

#define SRF08_NUM_ECHOES 17

/// States for the state machine
enum srf08_state {
    SRF08_STATE_INIT, // Only when first started
    SRF08_STATE_START_RANGING, // Activating ranging
    SRF08_STATE_WAITING // Waiting for ranging to complete
};

typedef struct {
    guint8 i2c_addr;
    enum srf08_state state; // State machine state
    GMutex *mutex;

    // As in the registers described in the docs
    guint8 range;
    guint8 analog_gain;

    // Data from the last ranging of the SRF08
    gboolean has_data;
    GTimeVal timestamp;
    guint8 light;
    guint16 echoes[SRF08_NUM_ECHOES];
} SRF08;


SRF08 *srf08_new(I2CBus *bus, int i2c_addr, int analog_gain, int range);
gboolean srf08_get_data(SRF08 *sonar, int *light, guint16 echoes[SRF08_NUM_ECHOES]);

