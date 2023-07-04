#define GPIO_LED_PORT  2
#define GPIO_LED_PIN   0
#define GPIO_LED  ((GPIO_LED_PORT - 1) * 32 + GPIO_LED_PIN)	// GPIO2_01

extern int kthread_led_loop(void *data);
