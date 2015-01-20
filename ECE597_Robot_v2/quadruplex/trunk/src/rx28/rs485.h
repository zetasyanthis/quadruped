
gint rs485_open(const gchar *dev);
gint rs485_send(const guint8 *buf, gsize len, gsize *bytes_written);
gint rs485_recv(guint8 *buf, gsize len, gsize *bytes_read);
gint rs485_getc(void);
gint rs485_close(void);

/* These are defined in include/asm-generic/ioctls.h */
#ifndef TIOCGRS485
   #define TIOCGRS485     0x542E
#endif
#ifndef TIOCSRS485
   #define TIOCSRS485     0x542F
#endif

// ugh, fix this:
typedef guint32 __u32;

// This is defined somewhere in the kernel.  As long as the first three fields
// are present it should work fine.
/* -- Settings used with the TIOCSRS485 & TIOCGRS485 ioctl */
struct serial_rs485_settings{
     __u32   flags;       /* RS485 feature flags */
#define SER_RS485_MODE       (3 << 0) /* Mask for mode. */
#define SER_RS485_MODE_DISABLED    (0 << 0)
#define SER_RS485_MODE_RTS      (1 << 0)
#define SER_RS485_MODE_DTR      (2 << 0)
#define SER_RS485_RTS_TX_LOW    (1 << 2) /* Inverted RTS   */
#define SER_RS485_DTR_TX_LOW    (1 << 3) /* Inverted DTR   */
     __u32   delay_before_send;    /* Nanoseconds */
     __u32   delay_after_send;     /* Nanoseconds */
     __u32   padding[5];             /* Memory is cheap, new structs
                    are a royal PITA .. */
};

#define RX_TIMEOUT 10 //ms
