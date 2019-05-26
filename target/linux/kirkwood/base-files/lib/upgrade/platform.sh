RAMFS_COPY_BIN='fw_printenv fw_setenv'
RAMFS_COPY_DATA='/etc/fw_env.config /var/lock/fw_printenv.lock'

REQUIRE_IMAGE_METADATA=1

platform_check_image() {
	local board=$(board_name)
	local magic="$(get_magic_long "$1")"

	case "$board" in
	"iptime,nas1")
		[ "$magic" != "6e617331" ] && {
			echo "Invalid image type."
			return 1
		}
		;;
	esac

	return 0
}

platform_do_upgrade() {
	local board="$(board_name)"

	case "$board" in
	"iptime,nas1")
		PART_NAME="iptime-fw"
		default_do_upgrade "$ARGV"
		;;
	"linksys,audi"|\
	"linksys,viper")
		platform_do_upgrade_linksys "$ARGV"
		;;
	*)
		nand_do_upgrade "$ARGV"
		;;
	esac
}
