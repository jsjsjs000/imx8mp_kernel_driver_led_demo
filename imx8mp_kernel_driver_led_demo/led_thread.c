#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/virtio.h>
#include <linux/ktime.h>
#include <linux/gpio.h>
#include <linux/rpmsg.h>
#include "led_thread.h"

int kthread_led_loop(void *data)
{
	ktime_t ktime;
  s64 ms, cycle;
	int led_on;

	if (gpio_request(GPIO_LED, "GPIO2_00") < 0)
	{
		pr_info("Can't control GPIO2_00");
		return 0;
	}

	gpio_direction_output(GPIO_LED, 1);

	while (!kthread_should_stop())
	{
		ktime = ktime_get();
		ms = ktime / 1000 / 1000;
    cycle = (ms / 100) % 20;
		led_on = cycle == 0 || cycle == 2;
		gpio_set_value(GPIO_LED, led_on);

		msleep(10);
	}

	gpio_free(GPIO_LED);

	return 0;
}
