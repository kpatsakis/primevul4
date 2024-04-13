static int __must_check ffs_do_descs(unsigned count, char *data, unsigned len,
				     ffs_entity_callback entity, void *priv)
{
	const unsigned _len = len;
	unsigned long num = 0;

	ENTER();

	for (;;) {
		int ret;

		if (num == count)
			data = NULL;

		/* Record "descriptor" entity */
		ret = entity(FFS_DESCRIPTOR, (u8 *)num, (void *)data, priv);
		if (unlikely(ret < 0)) {
			pr_debug("entity DESCRIPTOR(%02lx); ret = %d\n",
				 num, ret);
			return ret;
		}

		if (!data)
			return _len - len;

		ret = ffs_do_single_desc(data, len, entity, priv);
		if (unlikely(ret < 0)) {
			pr_debug("%s returns %d\n", __func__, ret);
			return ret;
		}

		len -= ret;
		data += ret;
		++num;
	}
}
