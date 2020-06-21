#
# Copyright (C) 2020 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=gpio-pca9570
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define KernelPackage/gpio-pca9570
  SUBMENU:=Other modules
  TITLE:=PCA9570 I2C GPO expander
  DEPENDS:=@GPIO_SUPPORT +kmod-i2c-core
  FILES:=$(PKG_BUILD_DIR)/gpio-pca9570.ko
  AUTOLOAD:=$(call AutoLoad,55,gpio-pca9570,1)
  KCONFIG:=
endef

define KernelPackage/gpio-pca9570/description
  Kernel module for PCA9570 I2C GPO expander.
endef

EXTRA_KCONFIG:= \
	CONFIG_GPIO_PCA9570=m

EXTRA_CFLAGS:= \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=m,%,$(filter %=m,$(EXTRA_KCONFIG)))) \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=y,%,$(filter %=y,$(EXTRA_KCONFIG)))) \

MAKE_OPTS:= \
	$(KERNEL_MAKE_FLAGS) \
	M="$(PKG_BUILD_DIR)" \
	EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
	$(EXTRA_KCONFIG)

define Build/Compile
	 $(MAKE) -C "$(LINUX_DIR)" \
		$(MAKE_OPTS) \
		modules
endef

$(eval $(call KernelPackage,gpio-pca9570))
