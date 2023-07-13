~/linux-imx-phytec-v5.10.72_2.2.0-phy9/arch/arm64/boot/dts/freescale/
imx8mp-phyboard-pollux-rdk-rpmsg-without-i2c2.dts
imx8mp-phyboard-pollux-rdk-rpmsg.dts
imx8mp-phyboard-pollux-without-i2c2.dtsi

imx8mp-phyboard-pollux-rdk-rpmsg-without-i2c2-gpio2_1.dts
imx8mp-phyboard-pollux-without-i2c2-gpio2_1.dtsi


~/linux-imx-phytec-v5.10.72_2.2.0-phy9/arch/arm64/boot/dts/freescale/Makefile
# ------------------ add ------------------
dtb-$(CONFIG_ARCH_MXC) += imx8mp-phyboard-pollux-rdk-rpmsg.dtb
dtb-$(CONFIG_ARCH_MXC) += imx8mp-phyboard-pollux-rdk-rpmsg-without-i2c2.dtb
# -----------------------------------------

# compile kernel
source /opt/ampliphy-vendor-xwayland/BSP-Yocto-NXP-i.MX8MP-PD22.1.1/environment-setup-cortexa53-crypto-phytec-linux
cd ~/linux-imx-phytec-v5.10.72_2.2.0-phy9
make -j16

# copy to sdcard to first /boot partition:
# file: imx8mp-phyboard-pollux-rdk-rpmsg.dtb
cp ~/linux-imx-phytec-v5.10.72_2.2.0-phy9/arch/arm64/boot/dts/freescale/imx8mp-phyboard-pollux-rdk-rpmsg.dtb /media/$USER/boot
# file: imx8mp-phyboard-pollux-rdk-rpmsg-without-i2c2.dtb
cp ~/linux-imx-phytec-v5.10.72_2.2.0-phy9/arch/arm64/boot/dts/freescale/imx8mp-phyboard-pollux-rdk-rpmsg-without-i2c2.dtb /media/$USER/boot
# file: imx8mp-phyboard-pollux-rdk-rpmsg-without-i2c2-gpio2_1.dtb
cp ~/linux-imx-phytec-v5.10.72_2.2.0-phy9/arch/arm64/boot/dts/freescale/imx8mp-phyboard-pollux-rdk-rpmsg-without-i2c2-gpio2_1.dtb /media/$USER/boot

umount /media/$USER/boot; umount /media/$USER/root

# u-boot - set device tree file
setenv fdt_file imx8mp-phyboard-pollux-rdk-rpmsg.dtb
setenv fdt_file imx8mp-phyboard-pollux-rdk-rpmsg-without-i2c2.dtb
setenv fdt_file imx8mp-phyboard-pollux-rdk-rpmsg-without-i2c2-gpio2_1.dtb
saveenv
reset
