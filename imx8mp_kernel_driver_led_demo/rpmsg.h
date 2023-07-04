#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/virtio.h>
#include <linux/rpmsg.h>

#define LED_COMMAND_UNDEFINED  255

typedef struct
{
	s32 time_milliseconds;
	u8 led_r;
	u8 led_g;
	u8 led_b;
	u8 led_mode;
} rpmsg_message_t;

#define LED_COMMAND_OFF       0
#define LED_COMMAND_LIGHT_25  1
#define LED_COMMAND_LIGHT_50  2
#define LED_COMMAND_LIGHT_100 3

enum led_mode_t { LED_MODE_AUTO, LED_MODE_MANUAL };

extern volatile rpmsg_message_t rpmsg_message;

enum command_t { COMMAND_NONE, COMMAND_SEND_LEDS_STATES, COMMAND_SEND_LEDS_MODE };
extern enum command_t command_from_procfs;

extern int rpmsg_init(struct rpmsg_device *rpdev);
extern int rpmsg_cb(struct rpmsg_device *rpdev, void *data, int len, void *priv, u32 src);
extern void rpmsg_deinit(void);
extern void rpmsg_enqueue_command_to_rpmsg(enum command_t command, u8 led_r, u8 led_g, u8 led_b,
		enum led_mode_t led_mode);
