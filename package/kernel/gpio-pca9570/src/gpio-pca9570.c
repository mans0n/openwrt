// SPDX-License-Identifier: GPL-2.0-only
/*
 * Driver for PCA9570 I2C GPO expander
 * 
 * Copyright 2020 Sungbo Eo <mans0n@gorani.run>
 *
 * Based on TPIC2810 driver
 * Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
 *	Andrew F. Davis <afd@ti.com>
 */

#include <linux/gpio/driver.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/mutex.h>

/**
 * struct pca9570 - GPIO driver data
 * @chip: GPIO controller chip
 * @client: I2C device pointer
 * @buffer: Buffer for device register
 * @lock: Protects write sequences
 */
struct pca9570 {
	struct gpio_chip chip;
	struct i2c_client *client;
	u8 buffer;
	struct mutex lock;
};

static void pca9570_set(struct gpio_chip *chip, unsigned offset, int value);

static int pca9570_get_direction(struct gpio_chip *chip,
				  unsigned offset)
{
	/* This device always output */
	return 0;
}

static int pca9570_direction_input(struct gpio_chip *chip,
				    unsigned offset)
{
	/* This device is output only */
	return -EINVAL;
}

static int pca9570_direction_output(struct gpio_chip *chip,
				     unsigned offset, int value)
{
	/* This device always output */
	pca9570_set(chip, offset, value);
	return 0;
}

static void pca9570_set_mask_bits(struct gpio_chip *chip, u8 mask, u8 bits)
{
	struct pca9570 *gpio = gpiochip_get_data(chip);
	u8 buffer;
	int err;

	mutex_lock(&gpio->lock);

	buffer = gpio->buffer & ~mask;
	buffer |= (mask & bits);

	err = i2c_smbus_write_byte(gpio->client, buffer);
	if (!err)
		gpio->buffer = buffer;

	mutex_unlock(&gpio->lock);
}

static void pca9570_set(struct gpio_chip *chip, unsigned offset, int value)
{
	pca9570_set_mask_bits(chip, BIT(offset), value ? BIT(offset) : 0);
}

static void pca9570_set_multiple(struct gpio_chip *chip, unsigned long *mask,
				  unsigned long *bits)
{
	pca9570_set_mask_bits(chip, *mask, *bits);
}

static const struct gpio_chip template_chip = {
	.label			= "pca9570",
	.owner			= THIS_MODULE,
	.get_direction		= pca9570_get_direction,
	.direction_input	= pca9570_direction_input,
	.direction_output	= pca9570_direction_output,
	.set			= pca9570_set,
	.set_multiple		= pca9570_set_multiple,
	.base			= -1,
	.ngpio			= 4,
	.can_sleep		= true,
};

static const struct of_device_id pca9570_of_match_table[] = {
	{ .compatible = "nxp,pca9570" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, pca9570_of_match_table);

static int pca9570_probe(struct i2c_client *client,
			  const struct i2c_device_id *id)
{
	struct pca9570 *gpio;
	int ret;

	gpio = devm_kzalloc(&client->dev, sizeof(*gpio), GFP_KERNEL);
	if (!gpio)
		return -ENOMEM;

	i2c_set_clientdata(client, gpio);

	gpio->chip = template_chip;
	gpio->chip.parent = &client->dev;

	gpio->client = client;

	mutex_init(&gpio->lock);

	ret = gpiochip_add_data(&gpio->chip, gpio);
	if (ret < 0) {
		dev_err(&client->dev, "Unable to register gpiochip\n");
		return ret;
	}

	return 0;
}

static int pca9570_remove(struct i2c_client *client)
{
	struct pca9570 *gpio = i2c_get_clientdata(client);

	gpiochip_remove(&gpio->chip);

	return 0;
}

static const struct i2c_device_id pca9570_id_table[] = {
	{ "pca9570", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(i2c, pca9570_id_table);

static struct i2c_driver pca9570_driver = {
	.driver = {
		.name = "pca9570",
		.of_match_table = pca9570_of_match_table,
	},
	.probe = pca9570_probe,
	.remove = pca9570_remove,
	.id_table = pca9570_id_table,
};
module_i2c_driver(pca9570_driver);

MODULE_AUTHOR("Sungbo Eo <mans0n@gorani.run>");
MODULE_DESCRIPTION("GPIO expander driver for PCA9570");
MODULE_LICENSE("GPL v2");
