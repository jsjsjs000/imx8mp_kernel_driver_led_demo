#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/virtio.h>
#include <linux/ktime.h>
#include <linux/gpio.h>
#include <linux/rpmsg.h>
#include "rpmsg.h"
#include "led_thread.h"
#include "procfs.h"

static struct task_struct *kthread_led;

static int rpmsg_connector_probe(struct rpmsg_device *rpdev)
{
	int err;
	dev_info(&rpdev->dev, "rpmsg connector driver is installed\n");

	rpmsg_init(rpdev);

	kthread_led = kthread_create(kthread_led_loop, NULL, "rpmsg_connector_kthread_led");
	wake_up_process(kthread_led);

	err = procfs_init();
	return err;
}

static int rpmsg_connector_cb(struct rpmsg_device *rpdev, void *data, int len, void *priv, u32 src)
{
	return rpmsg_cb(rpdev, data, len, priv, src);
}

static void rpmsg_connector_remove(struct rpmsg_device *rpdev)
{
	procfs_deinit();
	rpmsg_deinit();
	kthread_stop(kthread_led);
	dev_info(&rpdev->dev, "rpmsg connector driver is removed\n");
}

static struct rpmsg_device_id rpmsg_driver_connector_id_table[] =
{
	{ .name	= "rpmsg-openamp-channel-0" },
	{ .name	= "rpmsg-openamp-channel-1" },
	{ },
};

static struct rpmsg_driver rpmsg_connector_driver =
{
	.drv.name	= KBUILD_MODNAME,
	.drv.owner	= THIS_MODULE,
	.id_table	= rpmsg_driver_connector_id_table,
	.probe		= rpmsg_connector_probe,
	.callback	= rpmsg_connector_cb,
	.remove		= rpmsg_connector_remove,
};

static int __init init(void)
{
	return register_rpmsg_driver(&rpmsg_connector_driver);
}

static void __exit fini(void)
{
	unregister_rpmsg_driver(&rpmsg_connector_driver);
}

module_init(init);
module_exit(fini);

MODULE_AUTHOR("PCO jarsulk");
MODULE_DESCRIPTION("iMX virtio remote processor messaging connector driver");
MODULE_LICENSE("GPL v2");

/*

# setup environment:
source /opt/ampliphy-vendor-xwayland/BSP-Yocto-NXP-i.MX8MP-PD22.1.1/environment-setup-cortexa53-crypto-phytec-linux
cd ~/linux-imx-phytec-v5.10.72_2.2.0-phy9

# compile, upload to i.MX8 and restart i.MX8:
make -j16 && scp ~/linux-imx-phytec-v5.10.72_2.2.0-phy9/drivers/rpmsg/imx_rpmsg_connector.ko root@192.168.30.11:/lib/modules/5.10.72-bsp-yocto-nxp-i.mx8mp-pd22.1.1/kernel/drivers/rpmsg/ && ssh root@192.168.30.11 'reboot'

*/
