static int __must_check ffs_do_os_descs(unsigned count,
					char *data, unsigned len,
					ffs_os_desc_callback entity, void *priv)
{
	const unsigned _len = len;
	unsigned long num = 0;

	ENTER();

	for (num = 0; num < count; ++num) {
		int ret;
		enum ffs_os_desc_type type;
		u16 feature_count;
		struct usb_os_desc_header *desc = (void *)data;

		if (len < sizeof(*desc))
			return -EINVAL;

		/*
		 * Record "descriptor" entity.
		 * Process dwLength, bcdVersion, wIndex, get b/wCount.
		 * Move the data pointer to the beginning of extended
		 * compatibilities proper or extended properties proper
		 * portions of the data
		 */
		if (le32_to_cpu(desc->dwLength) > len)
			return -EINVAL;

		ret = __ffs_do_os_desc_header(&type, desc);
		if (unlikely(ret < 0)) {
			pr_debug("entity OS_DESCRIPTOR(%02lx); ret = %d\n",
				 num, ret);
			return ret;
		}
		/*
		 * 16-bit hex "?? 00" Little Endian looks like 8-bit hex "??"
		 */
		feature_count = le16_to_cpu(desc->wCount);
		if (type == FFS_OS_DESC_EXT_COMPAT &&
		    (feature_count > 255 || desc->Reserved))
				return -EINVAL;
		len -= ret;
		data += ret;

		/*
		 * Process all function/property descriptors
		 * of this Feature Descriptor
		 */
		ret = ffs_do_single_os_desc(data, len, type,
					    feature_count, entity, priv, desc);
		if (unlikely(ret < 0)) {
			pr_debug("%s returns %d\n", __func__, ret);
			return ret;
		}

		len -= ret;
		data += ret;
	}
	return _len - len;
}
