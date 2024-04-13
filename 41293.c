static int tg3_ape_send_event(struct tg3 *tp, u32 event)
{
	int err;
	u32 apedata;

	apedata = tg3_ape_read32(tp, TG3_APE_SEG_SIG);
	if (apedata != APE_SEG_SIG_MAGIC)
		return -EAGAIN;

	apedata = tg3_ape_read32(tp, TG3_APE_FW_STATUS);
	if (!(apedata & APE_FW_STATUS_READY))
		return -EAGAIN;

	/* Wait for up to 1 millisecond for APE to service previous event. */
	err = tg3_ape_event_lock(tp, 1000);
	if (err)
		return err;

	tg3_ape_write32(tp, TG3_APE_EVENT_STATUS,
			event | APE_EVENT_STATUS_EVENT_PENDING);

	tg3_ape_unlock(tp, TG3_APE_LOCK_MEM);
	tg3_ape_write32(tp, TG3_APE_EVENT, APE_EVENT_1);

	return 0;
}
