
/*
 * ipTIME A804NS-MU support
 *
 * Based on the TP-Link Archer C60 v1 board support code
 * Copyright (c) 2019 mans0n <mans0n@gorani.run>
 * Copyright (C) 2017 Henryk Heisig <hyniu@o2.pl>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 */

#include <linux/platform_device.h>
#include <linux/ath9k_platform.h>
#include <linux/ar8216_platform.h>
#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "dev-m25p80.h"
#include "machtypes.h"
#include "pci.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-spi.h"
#include "dev-usb.h"
#include "dev-wmac.h"

#define A804NM_GPIO_LED_CPU		1
#define A804NM_GPIO_LED_WLAN		19

#define A804NM_GPIO_BTN_RESET		14
#define A804NM_GPIO_BTN_WPS		14
#define A804NM_KEYS_POLL_INTERVAL	20     /* msecs */
#define A804NM_KEYS_DEBOUNCE_INTERVAL	(3 * A804NM_KEYS_POLL_INTERVAL)

#define A804NM_MAC0_OFFSET		0
#define A804NM_MAC1_OFFSET		6
#define A804NM_WMAC_CALDATA_OFFSET	0x1000
#define A804NM_PCI_CALDATA_OFFSET	0x5000

static struct gpio_led a804nm_leds_gpio[] __initdata = {
	{
		.name		= "a804nm:blue:cpu",
		.gpio		= A804NM_GPIO_LED_CPU,
		.active_low	= 1,
	},
	{
		.name		= "a804nm:blue:wlan",
		.gpio		= A804NM_GPIO_LED_WLAN,
		.active_low	= 1,
	},
};

static struct gpio_keys_button a804nm_gpio_keys[] __initdata = {
	{
		.desc		= "WPS button",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = A804NM_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= A804NM_GPIO_BTN_WPS,
		.active_low	= 1,
	},
	// Reset button uses the same gpio pin as WPS button,
	// so it cannot be declared at the same time.
};

static void __init a804nm_setup(void)
{
	u8 *art = (u8 *) KSEG1ADDR(0x1fff0000);

	ath79_register_m25p80(NULL);

	ath79_register_leds_gpio(-1, ARRAY_SIZE(a804nm_leds_gpio),
				 a804nm_leds_gpio);

	ath79_register_gpio_keys_polled(-1, A804NM_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(a804nm_gpio_keys),
					a804nm_gpio_keys);

	ath79_register_mdio(0, 0x0);
	ath79_register_mdio(1, 0x0);

	ath79_init_mac(ath79_eth0_data.mac_addr, art + A804NM_MAC0_OFFSET, 0);
	ath79_init_mac(ath79_eth1_data.mac_addr, art + A804NM_MAC1_OFFSET, 0);

	/* WAN port */
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ath79_eth0_data.speed = SPEED_100;
	ath79_eth0_data.duplex = DUPLEX_FULL;
	ath79_eth0_data.phy_mask = BIT(4);
	ath79_register_eth(0);

	/* LAN ports */
	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_GMII;
	ath79_eth1_data.speed = SPEED_1000;
	ath79_eth1_data.duplex = DUPLEX_FULL;
	ath79_switch_data.phy_poll_mask |= BIT(4);
	ath79_switch_data.phy4_mii_en = 1;
	ath79_register_eth(1);

	ath79_register_wmac(art + A804NM_WMAC_CALDATA_OFFSET, NULL);
	ath79_register_pci();
	ath79_register_usb();
}

MIPS_MACHINE(ATH79_MACH_A804NM, "A804NM", "ipTIME A804NS-MU",
	     a804nm_setup);
