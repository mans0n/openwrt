define Build/tnie_dw02-412h_header
  dd if=$@ of=$@.new bs=4 count=1 2>/dev/null
  dd if=/dev/zero of=$@.new bs=8 count=1 oflag=append conv=notrunc 2>/dev/null
  dd if=$@ of=$@.new skip=8 iflag=skip_bytes oflag=append conv=notrunc 2>/dev/null
  ( \
    header_crc="$$(dd if=$@.new bs=68 count=1 2>/dev/null | gzip -c | tail -c 8 | hexdump -v -n 4 -e '1/4 "%02x"')"; \
    echo -ne "$$(echo $$header_crc | sed 's/../\\x&/g')" | dd of=$@.new bs=4 seek=1 conv=notrunc 2>/dev/null; \
  )
  mv $@.new $@
endef

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

define Device/tnie_dw02-412h
  ATH_SOC := qca9557
  DEVICE_VENDOR := T&Ie
  DEVICE_MODEL := DW02-412H
  DEVICE_PACKAGES := kmod-ath10k-ct ath10k-firmware-qca988x-ct kmod-usb2
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  KERNEL_SIZE = 4096k
  IMAGE_SIZE := 131072k
  UIMAGE_NAME := ISQ-4000
  KERNEL := kernel-bin | append-dtb | lzma | uImage lzma | tnie_dw02-412h_header
  KERNEL_INITRAMFS := $$(KERNEL)
  IMAGES += factory.bin
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  IMAGE/factory.bin := append-kernel | pad-to $$$$(KERNEL_SIZE) | append-ubi | check-size $$$$(IMAGE_SIZE)
  UBINIZE_OPTS := -E 5
endef
TARGET_DEVICES += tnie_dw02-412h
