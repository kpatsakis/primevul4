static int __ffs_func_bind_do_os_desc(enum ffs_os_desc_type type,
				      struct usb_os_desc_header *h, void *data,
				      unsigned len, void *priv)
{
	struct ffs_function *func = priv;
	u8 length = 0;

	switch (type) {
	case FFS_OS_DESC_EXT_COMPAT: {
		struct usb_ext_compat_desc *desc = data;
		struct usb_os_desc_table *t;

		t = &func->function.os_desc_table[desc->bFirstInterfaceNumber];
		t->if_id = func->interfaces_nums[desc->bFirstInterfaceNumber];
		memcpy(t->os_desc->ext_compat_id, &desc->CompatibleID,
		       ARRAY_SIZE(desc->CompatibleID) +
		       ARRAY_SIZE(desc->SubCompatibleID));
		length = sizeof(*desc);
	}
		break;
	case FFS_OS_DESC_EXT_PROP: {
		struct usb_ext_prop_desc *desc = data;
		struct usb_os_desc_table *t;
		struct usb_os_desc_ext_prop *ext_prop;
		char *ext_prop_name;
		char *ext_prop_data;

		t = &func->function.os_desc_table[h->interface];
		t->if_id = func->interfaces_nums[h->interface];

		ext_prop = func->ffs->ms_os_descs_ext_prop_avail;
		func->ffs->ms_os_descs_ext_prop_avail += sizeof(*ext_prop);

		ext_prop->type = le32_to_cpu(desc->dwPropertyDataType);
		ext_prop->name_len = le16_to_cpu(desc->wPropertyNameLength);
		ext_prop->data_len = le32_to_cpu(*(u32 *)
			usb_ext_prop_data_len_ptr(data, ext_prop->name_len));
		length = ext_prop->name_len + ext_prop->data_len + 14;

		ext_prop_name = func->ffs->ms_os_descs_ext_prop_name_avail;
		func->ffs->ms_os_descs_ext_prop_name_avail +=
			ext_prop->name_len;

		ext_prop_data = func->ffs->ms_os_descs_ext_prop_data_avail;
		func->ffs->ms_os_descs_ext_prop_data_avail +=
			ext_prop->data_len;
		memcpy(ext_prop_data,
		       usb_ext_prop_data_ptr(data, ext_prop->name_len),
		       ext_prop->data_len);
		/* unicode data reported to the host as "WCHAR"s */
		switch (ext_prop->type) {
		case USB_EXT_PROP_UNICODE:
		case USB_EXT_PROP_UNICODE_ENV:
		case USB_EXT_PROP_UNICODE_LINK:
		case USB_EXT_PROP_UNICODE_MULTI:
			ext_prop->data_len *= 2;
			break;
		}
		ext_prop->data = ext_prop_data;

		memcpy(ext_prop_name, usb_ext_prop_name_ptr(data),
		       ext_prop->name_len);
		/* property name reported to the host as "WCHAR"s */
		ext_prop->name_len *= 2;
		ext_prop->name = ext_prop_name;

		t->os_desc->ext_prop_len +=
			ext_prop->name_len + ext_prop->data_len + 14;
		++t->os_desc->ext_prop_count;
		list_add_tail(&ext_prop->entry, &t->os_desc->ext_prop);
	}
		break;
	default:
		pr_vdebug("unknown descriptor: %d\n", type);
	}

	return length;
}
