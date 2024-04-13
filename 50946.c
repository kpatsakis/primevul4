static int __ffs_do_os_desc_header(enum ffs_os_desc_type *next_type,
				   struct usb_os_desc_header *desc)
{
	u16 bcd_version = le16_to_cpu(desc->bcdVersion);
	u16 w_index = le16_to_cpu(desc->wIndex);

	if (bcd_version != 1) {
		pr_vdebug("unsupported os descriptors version: %d",
			  bcd_version);
		return -EINVAL;
	}
	switch (w_index) {
	case 0x4:
		*next_type = FFS_OS_DESC_EXT_COMPAT;
		break;
	case 0x5:
		*next_type = FFS_OS_DESC_EXT_PROP;
		break;
	default:
		pr_vdebug("unsupported os descriptor type: %d", w_index);
		return -EINVAL;
	}

	return sizeof(*desc);
}
