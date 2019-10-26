define Device/7623a-iptime-a8004ns-m-emmc
  UIMAGE_NAME := a8004nm
  DEVICE_TITLE := MTK7623a ipTIME A8004NS-M (eMMC)
  DEVICE_DTS := mt7623a-iptime-a8004ns-m-emmc
  DEVICE_PACKAGES := mkf2fs e2fsprogs kmod-fs-vfat kmod-nls-cp437 kmod-nls-iso8859-1 kmod-mmc \
    kmod-mt7615e kmod-usb-ledtrig-usbport kmod-hwmon-core kmod-hwmon-gpiofan
  SUPPORTED_DEVICES := iptime,a8004ns-m-emmc
  IMAGES := sysupgrade-emmc.bin.gz
  IMAGE/sysupgrade-emmc.bin.gz := sysupgrade-emmc | gzip | append-metadata
endef

TARGET_DEVICES += 7623a-iptime-a8004ns-m-emmc

define Device/7623a-unielec-u7623-02-emmc-512m
  DEVICE_TITLE := MTK7623a UniElec U7623-02 (eMMC/512MB RAM)
  DEVICE_DTS := mt7623a-unielec-u7623-02-emmc-512M
  DEVICE_PACKAGES := mkf2fs e2fsprogs kmod-fs-vfat kmod-nls-cp437 kmod-nls-iso8859-1 kmod-mmc
  SUPPORTED_DEVICES := unielec,u7623-02-emmc-512m
  IMAGES := sysupgrade-emmc.bin.gz
  IMAGE/sysupgrade-emmc.bin.gz := sysupgrade-emmc | gzip | append-metadata
endef

TARGET_DEVICES += 7623a-unielec-u7623-02-emmc-512m

define Device/7623n-bananapi-bpi-r2
  DEVICE_TITLE := MTK7623n BananaPi R2
  DEVICE_DTS := mt7623n-bananapi-bpi-r2
endef

TARGET_DEVICES += 7623n-bananapi-bpi-r2
