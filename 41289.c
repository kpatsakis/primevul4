static int tg3_ape_event_lock(struct tg3 *tp, u32 timeout_us)
{
	u32 apedata;

	while (timeout_us) {
		if (tg3_ape_lock(tp, TG3_APE_LOCK_MEM))
			return -EBUSY;

		apedata = tg3_ape_read32(tp, TG3_APE_EVENT_STATUS);
		if (!(apedata & APE_EVENT_STATUS_EVENT_PENDING))
			break;

		tg3_ape_unlock(tp, TG3_APE_LOCK_MEM);

		udelay(10);
		timeout_us -= (timeout_us > 10) ? 10 : timeout_us;
	}

	return timeout_us ? 0 : -EBUSY;
}
