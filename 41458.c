static void tg3_probe_ncsi(struct tg3 *tp)
{
	u32 apedata;

	apedata = tg3_ape_read32(tp, TG3_APE_SEG_SIG);
	if (apedata != APE_SEG_SIG_MAGIC)
		return;

	apedata = tg3_ape_read32(tp, TG3_APE_FW_STATUS);
	if (!(apedata & APE_FW_STATUS_READY))
		return;

	if (tg3_ape_read32(tp, TG3_APE_FW_FEATURES) & TG3_APE_FW_FEATURE_NCSI)
		tg3_flag_set(tp, APE_HAS_NCSI);
}
