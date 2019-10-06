define Build/dongwon-tni_dw02-412h_header
  dd if=$@ of=$@.new bs=1 count=4
  echo -ne '\x00\x00\x00\x00\x00\x03\x00\x00' | dd of=$@.new bs=1 seek=4 conv=notrunc
  dd if=$@ of=$@.new bs=1 count=56 skip=8 seek=12 conv=notrunc
  crc32 $@.new | xxd -r -p | dd of=$@.new bs=1 seek=4 conv=notrunc
  dd if=$@ of=$@.new bs=4k skip=64 iflag=skip_bytes oflag=append conv=notrunc
  mv $@.new $@
endef

define Device/dongwon-tni_dw02-412h
  ATH_SOC := qca9557
  DEVICE_VENDOR := Dongwon TNI
  DEVICE_MODEL := DW02-412H
  DEVICE_PACKAGES := kmod-ath10k-ct ath10k-firmware-qca988x-ct kmod-usb2
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  KERNEL_SIZE = 2048k
  IMAGE_SIZE := 131072k
  UIMAGE_NAME := ISQ-4000
  KERNEL := kernel-bin | append-dtb | lzma | uImage lzma | dongwon-tni_dw02-412h_header
  KERNEL_INITRAMFS := $$(KERNEL)
  IMAGES := sysupgrade.tar
  IMAGE/sysupgrade.tar := sysupgrade-tar
endef
TARGET_DEVICES += dongwon-tni_dw02-412h

define Device/glinet_gl-ar300m-nand
  ATH_SOC := qca9531
  DEVICE_VENDOR := GL.iNet
  DEVICE_MODEL := GL-AR300M
  DEVICE_VARIANT := NAND
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-storage kmod-usb-ledtrig-usbport
  KERNEL_SIZE := 2048k
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  VID_HDR_OFFSET := 512
  IMAGES += factory.ubi
  IMAGE/sysupgrade.bin := sysupgrade-tar
  IMAGE/factory.ubi := append-kernel | pad-to $$$$(KERNEL_SIZE) | append-ubi
endef
TARGET_DEVICES += glinet_gl-ar300m-nand
