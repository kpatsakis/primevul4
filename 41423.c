static int tg3_nvram_write_block_buffered(struct tg3 *tp, u32 offset, u32 len,
		u8 *buf)
{
	int i, ret = 0;

	for (i = 0; i < len; i += 4, offset += 4) {
		u32 page_off, phy_addr, nvram_cmd;
		__be32 data;

		memcpy(&data, buf + i, 4);
		tw32(NVRAM_WRDATA, be32_to_cpu(data));

		page_off = offset % tp->nvram_pagesize;

		phy_addr = tg3_nvram_phys_addr(tp, offset);

		nvram_cmd = NVRAM_CMD_GO | NVRAM_CMD_DONE | NVRAM_CMD_WR;

		if (page_off == 0 || i == 0)
			nvram_cmd |= NVRAM_CMD_FIRST;
		if (page_off == (tp->nvram_pagesize - 4))
			nvram_cmd |= NVRAM_CMD_LAST;

		if (i == (len - 4))
			nvram_cmd |= NVRAM_CMD_LAST;

		if ((nvram_cmd & NVRAM_CMD_FIRST) ||
		    !tg3_flag(tp, FLASH) ||
		    !tg3_flag(tp, 57765_PLUS))
			tw32(NVRAM_ADDR, phy_addr);

		if (tg3_asic_rev(tp) != ASIC_REV_5752 &&
		    !tg3_flag(tp, 5755_PLUS) &&
		    (tp->nvram_jedecnum == JEDEC_ST) &&
		    (nvram_cmd & NVRAM_CMD_FIRST)) {
			u32 cmd;

			cmd = NVRAM_CMD_WREN | NVRAM_CMD_GO | NVRAM_CMD_DONE;
			ret = tg3_nvram_exec_cmd(tp, cmd);
			if (ret)
				break;
		}
		if (!tg3_flag(tp, FLASH)) {
			/* We always do complete word writes to eeprom. */
			nvram_cmd |= (NVRAM_CMD_FIRST | NVRAM_CMD_LAST);
		}

		ret = tg3_nvram_exec_cmd(tp, nvram_cmd);
		if (ret)
			break;
	}
	return ret;
}
