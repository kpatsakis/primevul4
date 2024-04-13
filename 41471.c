static void tg3_read_bc_ver(struct tg3 *tp)
{
	u32 val, offset, start, ver_offset;
	int i, dst_off;
	bool newver = false;

	if (tg3_nvram_read(tp, 0xc, &offset) ||
	    tg3_nvram_read(tp, 0x4, &start))
		return;

	offset = tg3_nvram_logical_addr(tp, offset);

	if (tg3_nvram_read(tp, offset, &val))
		return;

	if ((val & 0xfc000000) == 0x0c000000) {
		if (tg3_nvram_read(tp, offset + 4, &val))
			return;

		if (val == 0)
			newver = true;
	}

	dst_off = strlen(tp->fw_ver);

	if (newver) {
		if (TG3_VER_SIZE - dst_off < 16 ||
		    tg3_nvram_read(tp, offset + 8, &ver_offset))
			return;

		offset = offset + ver_offset - start;
		for (i = 0; i < 16; i += 4) {
			__be32 v;
			if (tg3_nvram_read_be32(tp, offset + i, &v))
				return;

			memcpy(tp->fw_ver + dst_off + i, &v, sizeof(v));
		}
	} else {
		u32 major, minor;

		if (tg3_nvram_read(tp, TG3_NVM_PTREV_BCVER, &ver_offset))
			return;

		major = (ver_offset & TG3_NVM_BCVER_MAJMSK) >>
			TG3_NVM_BCVER_MAJSFT;
		minor = ver_offset & TG3_NVM_BCVER_MINMSK;
		snprintf(&tp->fw_ver[dst_off], TG3_VER_SIZE - dst_off,
			 "v%d.%02d", major, minor);
	}
}
