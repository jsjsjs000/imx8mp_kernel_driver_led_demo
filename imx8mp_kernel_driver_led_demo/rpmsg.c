#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/virtio.h>
#include <linux/ktime.h>
#include <linux/gpio.h>
#include <linux/rpmsg.h>
#include "rpmsg.h"

#define HELLO_MSG		"hello world!"

static struct task_struct *kthread_rpmsg;

enum command_t command_from_procfs = COMMAND_NONE;
int8_t send_led_r;
int8_t send_led_g;
int8_t send_led_b;
enum led_mode_t send_led_mode;

volatile rpmsg_message_t rpmsg_message;

static struct rpmsg_device *rpmsg_dev;

static int kthread_rpmsg_loop(void *data)
{
	int err;
	ktime_t ktime;
	rpmsg_message_t rpmsg_message;

		/// wait 6 seconds for boot linux and skip boot messages
	ktime = ktime_get() / 1000 / 1000;		/// milliseconds
	if (ktime < 10 * 1000)
		usleep_range(6 * 1000 * 1000, 6 * 1000 * 1000 + 1);

	dev_info(&rpmsg_dev->dev, "RPMsg New channel: 0x%x -> 0x%x!\n", rpmsg_dev->src, rpmsg_dev->dst);

		/// send a message to our remote processor, and tell remote processor about this channel
	err = rpmsg_send(rpmsg_dev->ept, HELLO_MSG, strlen(HELLO_MSG));
	if (err)
	{
		dev_err(&rpmsg_dev->dev, "rpmsg_send failed: %d\n", err);
		return err;
	}

	msleep(50); /// fix synchronization problem

	while (!kthread_should_stop())
	{
		ktime = ktime_get() / 1000 / 1000;		/// milliseconds
		rpmsg_message.time_milliseconds = (s32)ktime;

		rpmsg_message.led_r = LED_COMMAND_UNDEFINED;
		rpmsg_message.led_g = LED_COMMAND_UNDEFINED;
		rpmsg_message.led_b = LED_COMMAND_UNDEFINED;
		rpmsg_message.led_mode = LED_COMMAND_UNDEFINED;
		if (command_from_procfs == COMMAND_SEND_LEDS_STATES)
		{
			command_from_procfs = COMMAND_NONE;
			rpmsg_message.led_r = send_led_r;
			rpmsg_message.led_g = send_led_g;
			rpmsg_message.led_b = send_led_b;
			rpmsg_message.led_mode = LED_MODE_MANUAL;
		}
		else if (command_from_procfs == COMMAND_SEND_LEDS_MODE)
		{
			command_from_procfs = COMMAND_NONE;
			rpmsg_message.led_mode = send_led_mode;
		}

		err = rpmsg_sendto(rpmsg_dev->ept, (void*)(&rpmsg_message), sizeof(rpmsg_message), rpmsg_dev->dst);
		if (err)
		{
			dev_err(&rpmsg_dev->dev, "rpmsg_send failed: %d\n", err);
			return err;
		}
		// pr_info("RPMsg Send ktime %llu\n", ktime);

		msleep(50);
	}

	return 0;
}

int rpmsg_init(struct rpmsg_device *rpdev)
{
	rpmsg_dev = rpdev;
	kthread_rpmsg = kthread_create(kthread_rpmsg_loop, NULL, "rpmsg_connector_kthread_rpmsg");
	wake_up_process(kthread_rpmsg);
	return 0;
}

int rpmsg_cb(struct rpmsg_device *rpdev, void *data, int len, void *priv, u32 src)
{
	// int err;
	// rpmsg_message_t rpmsg_message = *(rpmsg_message_t*)data;

	rpmsg_message = *(rpmsg_message_t*)data;

	// pr_info("RPMSg Received: %d %d %d %d (src: 0x%x)\n",
	// 		rpmsg_message.led_r, rpmsg_message.led_g, rpmsg_message.led_b,
	// 		rpmsg_message.led_mode, src);

	// err = rpmsg_sendto(rpdev->ept, (void*)(&rpmsg_message), 4, src);
	// if (err)
	// 	dev_err(&rpdev->dev, "rpmsg_send failed: %d\n", err);

	// return err;

	// ktime_t ktime;
	// ktime = ktime_get_boottime() / 1000 / 1000;		/// milliseconds
	// rpmsg_message = ktime;
	// err = rpmsg_sendto(rpdev->ept, (void*)(&rpmsg_message), sizeof(rpmsg_message), src);
	// if (err)
	// 	dev_err(&rpdev->dev, "rpmsg_send failed: %d\n", err);
	// return err;

	return 0;
}

void rpmsg_deinit(void)
{
	kthread_stop(kthread_rpmsg);
}

void rpmsg_enqueue_command_to_rpmsg(enum command_t command, u8 led_r, u8 led_g, u8 led_b,
		enum led_mode_t led_mode)
{
	send_led_r = led_r;
	send_led_g = led_g;
	send_led_b = led_b;
	send_led_mode = led_mode;
	command_from_procfs = command;
}
