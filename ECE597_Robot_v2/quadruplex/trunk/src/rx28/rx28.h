struct packet {
   guint8 id;
   guint8 length;   /* This is the length as defined by the datasheet, len(param) + 2 */
   guint8 instruction;
   guint8 *param;
   guint8 checksum;
};

struct sync_write {
   guint8 id;
   guint8 *data;
};

#define RX28_RPM_TO_SPEED_UNITS (1.0 / 0.111)
#define RX28_ANGLE_UNITS_TO_REVOLUTIONS (300.0 / 1023.0 / 360.0)
#define PACK_U16(p, v) do { (p)[0] = (v) & 0xFF; (p)[1] = (v) >> 010; } while (0)

#define kRX28ErrorInstruction  (1<<6)
#define kRX28ErrorOverload     (1<<5)
#define kRX28ErrorChecksum     (1<<4)
#define kRX28ErrorRange        (1<<3)
#define kRX28ErrorOverheating  (1<<2)
#define kRX28ErrorAngleLimit   (1<<1)
#define kRX28ErrorInputVoltage (1<<0)

enum {
   kRX28InstrReadData  = 0x02,
   kRX28InstrWriteData = 0x03,
   kRX28InstrRegWrite  = 0x04,
   kRX28InstrAction    = 0x05,
   kRX28InstrPing      = 0x01,
   kRX28InstrReset     = 0x06,
   kRX28InstrSyncWrite = 0x83,
};

typedef enum {
   kRX28ModelNumber                             = 0x00,
   kRX28Version                                 = 0x02,
   kRX28ID                                      = 0x03,
   kRX28BaudRate                                = 0x04,
   kRX28ReturnDelayTime                         = 0x05,
   kRX28ClockwiseAngleLimit                     = 0x06,
   kRX28CounterClockwiseAngleLimit              = 0x08,
   kRX28HighestLimitTemperature                 = 0x0B,
   kRX28LowestLimitVoltage                      = 0x0C,
   kRX28HighestLimitVoltage                     = 0x0D,
   kRX28MaxTorque                               = 0x0E,
   kRX28StatusReturnLevel                       = 0x10,
   kRX28AlarmLED                                = 0x11,
   kRX28AlarmShutdown                           = 0x12,
   kRX28TorqueEnable                            = 0x18,
   kRX28LED                                     = 0x19,
   kRX28ClockwiseComplianceMargin               = 0x1A,
   kRX28CounterClockwiseComplianceMargin        = 0x1B,
   kRX28ClockwiseComplianceSlope                = 0x1C,
   kRX28CounterClockwiseComplianceSlope         = 0x1D,
   kRX28GoalPosition                            = 0x1E,
   kRX28MovingSpeed                             = 0x20,
   kRX28TorqueLimit                             = 0x22,
   kRX28PresentPosition                         = 0x24,
   kRX28PresentSpeed                            = 0x26,
   kRX28PresentLoad                             = 0x28,
   kRX28PresentVoltage                          = 0x2A,
   kRX28PresentTemperature                      = 0x2B,
   kRX28RegisteredInstruction                   = 0x2C,
   kRX28Moving                                  = 0x2E,
   kRX28Lock                                    = 0x2F,
   kRX28Punch                                   = 0x30,
   tRX28End,
} tRX28Register;

typedef enum { kRX28AccessR, kRX28AccessRW } tRX28Access;
typedef enum { kRX28SizeU8, kRX28SizeU16 } tRX28Size;

gint rx28_send_packet(struct packet *p, struct packet *status);
void rx28_print_packet(const struct packet *p);
gint rx28_receive_status_packet(struct packet *status);


gboolean rx28_ping(guint8 id, struct packet *status);

gint rx28_write_data(guint8 id, guint8 addr, const guint8 *buf, guint8 len);
gint rx28_write_data_byte(guint8 id, guint8 addr, guint8 byte);
gint rx28_write_data_word(guint8 id, guint8 addr, guint16 word);

gint rx28_read_data(guint8 id, guint8 addr, guint8 *buf, guint8 len);
gint rx28_read_data_byte(guint8 id, guint8 addr, guint8 *byte);
gint rx28_read_data_word(guint8 id, guint8 addr, guint16 *word);

gint rx28_reg_write(guint8 id, guint8 addr, const guint8 *buf, guint8 len);
gint rx28_reg_write_byte(guint8 id, guint8 addr, const guint8 byte);
gint rx28_reg_write_word(guint8 id, guint8 addr, const guint16 word);

gint rx28_execute_action(void);

gint rx28_sync_write(guint8 addr, struct sync_write *data, guint8 n_ids, guint8 len_per_id);
