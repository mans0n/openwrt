/*
 * kt DW02-412H board support
 *
 * Based on the Qualcomm Atheros AP135/AP136 reference board support code
 *   and the ZyXEL NBG6716/NBG6616 board support code
 * Copyright (c) 2012 Qualcomm Atheros
 * Copyright (c) 2012-2013 Gabor Juhos <juhosg@openwrt.org>
 * Copyright (c) 2013 Andre Valentin <avalentin@marcant.net>
 * Copyright (c) 2018 Sungbo Eo <mans0n@gorani.run>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#include <linux/platform_device.h>
#include <linux/ar8216_platform.h>
#include <linux/gpio.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/platform/ar934x_nfc.h>

#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "pci.h"
#include "dev-ap9x-pci.h"
#include "dev-gpio-buttons.h"
#include "dev-eth.h"
#include "dev-leds-gpio.h"
#include "dev-nfc.h"
#include "dev-m25p80.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"

#define DW02_412H_GPIO_LED_WAN			22
#define DW02_412H_GPIO_LED_WIFI			13

#define DW02_412H_GPIO_BTN_RESET		17
#define DW02_412H_GPIO_BTN_WPS			16

#define DW02_412H_KEYS_POLL_INTERVAL		20	/* msecs */
#define DW02_412H_KEYS_DEBOUNCE_INTERVAL	(3 * DW02_412H_KEYS_POLL_INTERVAL)

#define DW02_412H_MAC0_OFFSET			0
#define DW02_412H_MAC1_OFFSET			6
#define DW02_412H_WMAC_CALDATA_OFFSET		0x1000
#define DW02_412H_PCIE_CALDATA_OFFSET		0x5000

static struct gpio_led dw02_412h_leds_gpio[] __initdata = {
	{
		.name		= "dw02-412h:green:wan",
		.gpio		= DW02_412H_GPIO_LED_WAN,
		.active_low	= 1,
	},
	{
		.name		= "dw02-412h:green:wifi",
		.gpio		= DW02_412H_GPIO_LED_WIFI,
		.active_low	= 1,
	},
};

static struct gpio_keys_button dw02_412h_gpio_keys[] __initdata = {
	{
		.desc		= "RESET button",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = DW02_412H_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= DW02_412H_GPIO_BTN_RESET,
		.active_low	= 1,
	},
	{
		.desc		= "WPS button",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = DW02_412H_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= DW02_412H_GPIO_BTN_WPS,
		.active_low	= 1,
	},
};

static struct ar8327_pad_cfg dw02_412h_ar8327_pad0_cfg;
static struct ar8327_pad_cfg dw02_412h_ar8327_pad6_cfg;
static struct ar8327_led_cfg dw02_412h_ar8327_led_cfg;

static struct ar8327_platform_data dw02_412h_ar8327_data = {
	.pad0_cfg = &dw02_412h_ar8327_pad0_cfg,
	.pad6_cfg = &dw02_412h_ar8327_pad6_cfg,
	.port0_cfg = {
		.force_link = 1,
		.speed = AR8327_PORT_SPEED_1000,
		.duplex = 1,
		.txpause = 1,
		.rxpause = 1,
	},
	.port6_cfg = {
		.force_link = 1,
		.speed = AR8327_PORT_SPEED_1000,
		.duplex = 1,
		.txpause = 1,
		.rxpause = 1,
	},
	.led_cfg = &dw02_412h_ar8327_led_cfg
};

static struct mdio_board_info dw02_412h_mdio0_info[] = {
	{
		.bus_id = "ag71xx-mdio.0",
		.phy_addr = 0,
		.platform_data = &dw02_412h_ar8327_data,
	},
};

static void __init dw02_412h_setup(void)
{
	u32 leds_num = ARRAY_SIZE(dw02_412h_leds_gpio);
	struct gpio_led* leds = dw02_412h_leds_gpio;
	u32 keys_num = ARRAY_SIZE(dw02_412h_gpio_keys);
	struct gpio_keys_button* keys = dw02_412h_gpio_keys;

	u8 *art = (u8 *) KSEG1ADDR(0x1fff0000);
	u8 tmp_wmac[ETH_ALEN];

	/* GMAC0 of the AR8337 switch is connected to GMAC0 via RGMII */
	dw02_412h_ar8327_pad0_cfg.mode = AR8327_PAD_MAC_RGMII;
	dw02_412h_ar8327_pad0_cfg.txclk_delay_en = true;
	dw02_412h_ar8327_pad0_cfg.rxclk_delay_en = true;
	dw02_412h_ar8327_pad0_cfg.txclk_delay_sel = AR8327_CLK_DELAY_SEL1;
	dw02_412h_ar8327_pad0_cfg.rxclk_delay_sel = AR8327_CLK_DELAY_SEL2;
	dw02_412h_ar8327_pad0_cfg.mac06_exchange_dis = true;

	/* GMAC6 of the AR8337 switch is connected to GMAC1 via SGMII */
	dw02_412h_ar8327_pad6_cfg.mode = AR8327_PAD_MAC_SGMII;
	dw02_412h_ar8327_pad6_cfg.rxclk_delay_en = true;
	dw02_412h_ar8327_pad6_cfg.rxclk_delay_sel = AR8327_CLK_DELAY_SEL0;

	ath79_eth0_pll_data.pll_1000 = 0xa6000000;
	ath79_eth1_pll_data.pll_1000 = 0x03000101;

	dw02_412h_ar8327_led_cfg.open_drain = 0;
	dw02_412h_ar8327_led_cfg.led_ctrl0 = 0xffb7ffb7;
	dw02_412h_ar8327_led_cfg.led_ctrl1 = 0xffb7ffb7;
	dw02_412h_ar8327_led_cfg.led_ctrl2 = 0xffb7ffb7;
	dw02_412h_ar8327_led_cfg.led_ctrl3 = 0x03ffff00;

	ath79_register_m25p80(NULL);

	ath79_register_leds_gpio(-1, leds_num, leds);
	ath79_register_gpio_keys_polled(-1, DW02_412H_KEYS_POLL_INTERVAL,
					keys_num, keys);

	ath79_nfc_set_ecc_mode(AR934X_NFC_ECC_HW);
	ath79_register_nfc();

	ath79_register_usb();

	ath79_init_mac(&tmp_wmac[0], art + DW02_412H_MAC1_OFFSET, 2);

	ath79_register_pci();
	// ap91_pci_init(art + DW02_412H_PCIE_CALDATA_OFFSET, NULL);

	ath79_register_wmac(art + DW02_412H_WMAC_CALDATA_OFFSET, tmp_wmac);

	ath79_setup_qca955x_eth_cfg(QCA955X_ETH_CFG_RGMII_EN);

	ath79_register_mdio(0, 0x0);

	ath79_init_mac(ath79_eth0_data.mac_addr, art + DW02_412H_MAC0_OFFSET, 0);
	ath79_init_mac(ath79_eth1_data.mac_addr, art + DW02_412H_MAC1_OFFSET, 0);

	mdiobus_register_board_info(dw02_412h_mdio0_info,
				    ARRAY_SIZE(dw02_412h_mdio0_info));

	/* GMAC0 is connected to the RMGII interface */
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ath79_eth0_data.phy_mask = BIT(0);
	ath79_eth0_data.mii_bus_dev = &ath79_mdio0_device.dev;

	ath79_register_eth(0);

	/* GMAC1 is connected to the SGMII interface */
	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_SGMII;
	ath79_eth1_data.speed = SPEED_1000;
	ath79_eth1_data.duplex = DUPLEX_FULL;

	ath79_register_eth(1);
}

MIPS_MACHINE(ATH79_MACH_DW02_412H, "DW02-412H",
	     "kt DW02-412H",
	     dw02_412h_setup);
