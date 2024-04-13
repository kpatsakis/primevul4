static int __must_check ffs_do_single_os_desc(char *data, unsigned len,
					      enum ffs_os_desc_type type,
					      u16 feature_count,
					      ffs_os_desc_callback entity,
					      void *priv,
					      struct usb_os_desc_header *h)
{
	int ret;
	const unsigned _len = len;

	ENTER();

	/* loop over all ext compat/ext prop descriptors */
	while (feature_count--) {
		ret = entity(type, h, data, len, priv);
		if (unlikely(ret < 0)) {
			pr_debug("bad OS descriptor, type: %d\n", type);
			return ret;
		}
		data += ret;
		len -= ret;
	}
	return _len - len;
}
