static void tg3_read_mgmtfw_ver(struct tg3 *tp)
{
	u32 val, offset, start;
	int i, vlen;

	for (offset = TG3_NVM_DIR_START;
	     offset < TG3_NVM_DIR_END;
	     offset += TG3_NVM_DIRENT_SIZE) {
		if (tg3_nvram_read(tp, offset, &val))
			return;

		if ((val >> TG3_NVM_DIRTYPE_SHIFT) == TG3_NVM_DIRTYPE_ASFINI)
			break;
	}

	if (offset == TG3_NVM_DIR_END)
		return;

	if (!tg3_flag(tp, 5705_PLUS))
		start = 0x08000000;
	else if (tg3_nvram_read(tp, offset - 4, &start))
		return;

	if (tg3_nvram_read(tp, offset + 4, &offset) ||
	    !tg3_fw_img_is_valid(tp, offset) ||
	    tg3_nvram_read(tp, offset + 8, &val))
		return;

	offset += val - start;

	vlen = strlen(tp->fw_ver);

	tp->fw_ver[vlen++] = ',';
	tp->fw_ver[vlen++] = ' ';

	for (i = 0; i < 4; i++) {
		__be32 v;
		if (tg3_nvram_read_be32(tp, offset, &v))
			return;

		offset += sizeof(v);

		if (vlen > TG3_VER_SIZE - sizeof(v)) {
			memcpy(&tp->fw_ver[vlen], &v, TG3_VER_SIZE - vlen);
			break;
		}

		memcpy(&tp->fw_ver[vlen], &v, sizeof(v));
		vlen += sizeof(v);
	}
}
