/*
 *  LG U+ NAPL-5000 support
 *
 *  Based on the ALFA Network N2/N5 board support code
 *  Copyright (c) 2019 mans0n <mans0n@gorani.run>
 *  Copyright (C) 2011-2012 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <asm/mach-ath79/ar71xx_regs.h>
#include <asm/mach-ath79/ath79.h>

#include "common.h"
#include "dev-eth.h"
#include "dev-ap9x-pci.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "machtypes.h"

#define NAPL_5000_GPIO_POWER			12
#define NAPL_5000_GPIO_WLAN			502

#define NAPL_5000_GPIO_BTN_RESET		11

#define NAPL_5000_KEYS_POLL_INTERVAL		20	/* msecs */
#define NAPL_5000_KEYS_DEBOUNCE_INTERVAL	(3 * NAPL_5000_KEYS_POLL_INTERVAL)

#define NAPL_5000_MAC_OFFSET			0x110c
#define NAPL_5000_CALDATA_OFFSET		0x1000

static struct gpio_keys_button napl_5000_gpio_keys[] __initdata = {
	{
		.desc		= "Reset button",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = NAPL_5000_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= NAPL_5000_GPIO_BTN_RESET,
		.active_low	= 1,
	}
};

static struct gpio_led napl_5000_leds_gpio[] __initdata = {
	{
		.name		= "napl-5000:red:power",
		.gpio		= NAPL_5000_GPIO_POWER,
		.active_low	= 0,
	},
	{
		.name		= "napl-5000:green:wlan",
		.gpio		= NAPL_5000_GPIO_WLAN,
		.active_low	= 1,
	}
};

static void __init napl_5000_setup(void)
{
	u8 *art = (u8 *) KSEG1ADDR(0x1fff0000);
	u8 wlan_mac[ETH_ALEN];

	ath79_register_m25p80(NULL);

	ath79_register_leds_gpio(-1, ARRAY_SIZE(napl_5000_leds_gpio),
				 napl_5000_leds_gpio);

	ath79_register_gpio_keys_polled(-1, NAPL_5000_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(napl_5000_gpio_keys),
					napl_5000_gpio_keys);

	ath79_register_mdio(0, 0x0);

	ath79_init_mac(ath79_eth0_data.mac_addr, art + NAPL_5000_MAC_OFFSET, 3);
	ath79_init_mac(ath79_eth1_data.mac_addr, art + NAPL_5000_MAC_OFFSET, 0);

	/* WAN port */
	ath79_register_eth(0);
	/* LAN port */
	ath79_register_eth(1);

	ath79_init_mac(wlan_mac, art + NAPL_5000_MAC_OFFSET, 1);
	ap91_pci_init(art + NAPL_5000_CALDATA_OFFSET, wlan_mac);
}

MIPS_MACHINE(ATH79_MACH_NAPL_5000, "NAPL-5000", "LG U+ NAPL-5000",
	     napl_5000_setup);
