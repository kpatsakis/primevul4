static int __ffs_data_do_os_desc(enum ffs_os_desc_type type,
				 struct usb_os_desc_header *h, void *data,
				 unsigned len, void *priv)
{
	struct ffs_data *ffs = priv;
	u8 length;

	ENTER();

	switch (type) {
	case FFS_OS_DESC_EXT_COMPAT: {
		struct usb_ext_compat_desc *d = data;
		int i;

		if (len < sizeof(*d) ||
		    d->bFirstInterfaceNumber >= ffs->interfaces_count ||
		    d->Reserved1)
			return -EINVAL;
		for (i = 0; i < ARRAY_SIZE(d->Reserved2); ++i)
			if (d->Reserved2[i])
				return -EINVAL;

		length = sizeof(struct usb_ext_compat_desc);
	}
		break;
	case FFS_OS_DESC_EXT_PROP: {
		struct usb_ext_prop_desc *d = data;
		u32 type, pdl;
		u16 pnl;

		if (len < sizeof(*d) || h->interface >= ffs->interfaces_count)
			return -EINVAL;
		length = le32_to_cpu(d->dwSize);
		type = le32_to_cpu(d->dwPropertyDataType);
		if (type < USB_EXT_PROP_UNICODE ||
		    type > USB_EXT_PROP_UNICODE_MULTI) {
			pr_vdebug("unsupported os descriptor property type: %d",
				  type);
			return -EINVAL;
		}
		pnl = le16_to_cpu(d->wPropertyNameLength);
		pdl = le32_to_cpu(*(u32 *)((u8 *)data + 10 + pnl));
		if (length != 14 + pnl + pdl) {
			pr_vdebug("invalid os descriptor length: %d pnl:%d pdl:%d (descriptor %d)\n",
				  length, pnl, pdl, type);
			return -EINVAL;
		}
		++ffs->ms_os_descs_ext_prop_count;
		/* property name reported to the host as "WCHAR"s */
		ffs->ms_os_descs_ext_prop_name_len += pnl * 2;
		ffs->ms_os_descs_ext_prop_data_len += pdl;
	}
		break;
	default:
		pr_vdebug("unknown descriptor: %d\n", type);
		return -EINVAL;
	}
	return length;
}
