static int tg3_fw_img_is_valid(struct tg3 *tp, u32 offset)
{
	u32 val;

	if (tg3_nvram_read(tp, offset, &val) ||
	    (val & 0xfc000000) != 0x0c000000 ||
	    tg3_nvram_read(tp, offset + 4, &val) ||
	    val != 0)
		return 0;

	return 1;
}
