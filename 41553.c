static __be32 *tg3_vpd_readblock(struct tg3 *tp, u32 *vpdlen)
{
	int i;
	__be32 *buf;
	u32 offset = 0, len = 0;
	u32 magic, val;

	if (tg3_flag(tp, NO_NVRAM) || tg3_nvram_read(tp, 0, &magic))
		return NULL;

	if (magic == TG3_EEPROM_MAGIC) {
		for (offset = TG3_NVM_DIR_START;
		     offset < TG3_NVM_DIR_END;
		     offset += TG3_NVM_DIRENT_SIZE) {
			if (tg3_nvram_read(tp, offset, &val))
				return NULL;

			if ((val >> TG3_NVM_DIRTYPE_SHIFT) ==
			    TG3_NVM_DIRTYPE_EXTVPD)
				break;
		}

		if (offset != TG3_NVM_DIR_END) {
			len = (val & TG3_NVM_DIRTYPE_LENMSK) * 4;
			if (tg3_nvram_read(tp, offset + 4, &offset))
				return NULL;

			offset = tg3_nvram_logical_addr(tp, offset);
		}
	}

	if (!offset || !len) {
		offset = TG3_NVM_VPD_OFF;
		len = TG3_NVM_VPD_LEN;
	}

	buf = kmalloc(len, GFP_KERNEL);
	if (buf == NULL)
		return NULL;

	if (magic == TG3_EEPROM_MAGIC) {
		for (i = 0; i < len; i += 4) {
			/* The data is in little-endian format in NVRAM.
			 * Use the big-endian read routines to preserve
			 * the byte order as it exists in NVRAM.
			 */
			if (tg3_nvram_read_be32(tp, offset + i, &buf[i/4]))
				goto error;
		}
	} else {
		u8 *ptr;
		ssize_t cnt;
		unsigned int pos = 0;

		ptr = (u8 *)&buf[0];
		for (i = 0; pos < len && i < 3; i++, pos += cnt, ptr += cnt) {
			cnt = pci_read_vpd(tp->pdev, pos,
					   len - pos, ptr);
			if (cnt == -ETIMEDOUT || cnt == -EINTR)
				cnt = 0;
			else if (cnt < 0)
				goto error;
		}
		if (pos != len)
			goto error;
	}

	*vpdlen = len;

	return buf;

error:
	kfree(buf);
	return NULL;
}
