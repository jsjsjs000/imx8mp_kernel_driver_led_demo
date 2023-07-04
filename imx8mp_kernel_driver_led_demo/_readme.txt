~/linux-imx-phytec-v5.10.72_2.2.0-phy9/drivers/rpmsg/imx8mp_kernel_driver_led_demo/
~/linux-imx-phytec-v5.10.72_2.2.0-phy9/drivers/rpmsg/imx8mp_kernel_driver_led_demo/main.c
~/linux-imx-phytec-v5.10.72_2.2.0-phy9/drivers/rpmsg/imx8mp_kernel_driver_led_demo/imx8mp_kernel_driver_led_demo.ko

# copy kernel module to this repository
backup_dir=/home/p2119/jarsulk-pco/programs/imx8mp/a53/kernel_space/imx8mp_kernel_driver_led_demo/imx8mp_kernel_driver_led_demo/
cp ~/linux-imx-phytec-v5.10.72_2.2.0-phy9/drivers/rpmsg/imx8mp_kernel_driver_led_demo/{Makefile,main.c} $backup_dir
cp ~/linux-imx-phytec-v5.10.72_2.2.0-phy9/drivers/rpmsg/imx8mp_kernel_driver_led_demo/{rpmsg,led_thread,procfs,commands}.{c,h} $backup_dir

# add new module to kernel configuration
~/linux-imx-phytec-v5.10.72_2.2.0-phy9/.config
CONFIG_REMOTEPROC=y
CONFIG_IMX_REMOTEPROC=y
CONFIG_IMX_DSP_REMOTEPROC=y
CONFIG_IMX8MP_KERNEL_DRIVER_LED_DEMO=m

~/linux-imx-phytec-v5.10.72_2.2.0-phy9/drivers/rpmsg/Makefile
obj-$(CONFIG_IMX8MP_KERNEL_DRIVER_LED_DEMO)	+= imx8mp_kernel_driver_led_demo/

~/linux-imx-phytec-v5.10.72_2.2.0-phy9/drivers/rpmsg/imx8mp_kernel_driver_led_demo/Makefile
imx8mp_kernel_driver_led_demo-objs := main.o commands.o rpmsg.o led_thread.o procfs.o
obj-$(CONFIG_IMX8MP_KERNEL_DRIVER_LED_DEMO)	+= imx8mp_kernel_driver_led_demo.o

# help: compile multiple files in driver
# https://stackoverflow.com/questions/73373914/how-to-compile-multiple-files-together-when-one-of-them-is-a-linux-kernel-module

~/linux-imx-phytec-v5.10.72_2.2.0-phy9/drivers/rpmsg/Kconfig
config IMX8MP_KERNEL_DRIVER_LED_DEMO
	tristate "IMX8MP kernel driver LED demo - loadable modules only"
	default m
	depends on HAVE_IMX_RPMSG && m
	help
	  Jarsulk LED demo kernel driver - RPMSG

# compile kernel and modules
source /opt/ampliphy-vendor-xwayland/BSP-Yocto-NXP-i.MX8MP-PD22.1.1/environment-setup-cortexa53-crypto-phytec-linux
cd ~/linux-imx-phytec-v5.10.72_2.2.0-phy9
make -j16

# create kernel folder
ssh root@192.168.30.11 'mkdir /lib/modules/5.10.72-bsp-yocto-nxp-i.mx8mp-pd22.1.1/kernel/drivers/rpmsg/imx8mp_kernel_driver_led_demo'

# upload to i.MX8
scp ~/linux-imx-phytec-v5.10.72_2.2.0-phy9/drivers/rpmsg/imx8mp_kernel_driver_led_demo/imx8mp_kernel_driver_led_demo.ko root@192.168.30.11:/lib/modules/5.10.72-bsp-yocto-nxp-i.mx8mp-pd22.1.1/kernel/drivers/rpmsg/imx8mp_kernel_driver_led_demo/

# compile kernel and modules, upload module to i.MX8 and restart i.MX8
make -j16 && scp ~/linux-imx-phytec-v5.10.72_2.2.0-phy9/drivers/rpmsg/imx8mp_kernel_driver_led_demo/imx8mp_kernel_driver_led_demo.ko root@192.168.30.11:/lib/modules/5.10.72-bsp-yocto-nxp-i.mx8mp-pd22.1.1/kernel/drivers/rpmsg/imx8mp_kernel_driver_led_demo/ && ssh root@192.168.30.11 'reboot'

# compile kernel and modules, upload module to i.MX8, rmmod and insmod
make -j16 && scp ~/linux-imx-phytec-v5.10.72_2.2.0-phy9/drivers/rpmsg/imx8mp_kernel_driver_led_demo/imx8mp_kernel_driver_led_demo.ko root@192.168.30.11:/lib/modules/5.10.72-bsp-yocto-nxp-i.mx8mp-pd22.1.1/kernel/drivers/rpmsg/imx8mp_kernel_driver_led_demo/ && ssh root@192.168.30.11 'rmmod /lib/modules/5.10.72-bsp-yocto-nxp-i.mx8mp-pd22.1.1/kernel/drivers/rpmsg/imx8mp_kernel_driver_led_demo/imx8mp_kernel_driver_led_demo.ko; insmod /lib/modules/5.10.72-bsp-yocto-nxp-i.mx8mp-pd22.1.1/kernel/drivers/rpmsg/imx8mp_kernel_driver_led_demo/imx8mp_kernel_driver_led_demo.ko'

# test procfs
cat /proc/led_demo
echo "leds 0 0 3" > /proc/led_demo   # blue LED
echo "leds_mode 0" > /proc/led_demo  # auto mode

# run on i.MX8 Linux
insmod /lib/modules/5.10.72-bsp-yocto-nxp-i.mx8mp-pd22.1.1/kernel/drivers/rpmsg/imx8mp_kernel_driver_led_demo/imx8mp_kernel_driver_led_demo.ko
# or
depmod   # recreate modules dependency list
modprobe imx8mp_kernel_driver_led_demo

# stop on i.MX8 Linux
rmmod imx8mp_kernel_driver_led_demo.ko

# autorun module at boot
echo "imx8mp_kernel_driver_led_demo" >> /etc/modules-load.d/imx8mp_kernel_driver_led_demo.conf
cat /etc/modules-load.d/imx8mp_kernel_driver_led_demo.conf
depmod   # recreate modules dependency list
# rm /etc/modules-load.d/imx8mp_kernel_driver_led_demo.conf
