static void tg3_sd_scan_scratchpad(struct tg3 *tp, struct tg3_ocir *ocir)
{
	int i;

	for (i = 0; i < TG3_SD_NUM_RECS; i++, ocir++) {
		u32 off = i * TG3_OCIR_LEN, len = TG3_OCIR_LEN;

		tg3_ape_scratchpad_read(tp, (u32 *) ocir, off, len);
		off += len;

		if (ocir->signature != TG3_OCIR_SIG_MAGIC ||
		    !(ocir->version_flags & TG3_OCIR_FLAG_ACTIVE))
			memset(ocir, 0, TG3_OCIR_LEN);
	}
}
