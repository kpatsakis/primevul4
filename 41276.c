static void _tw32_flush(struct tg3 *tp, u32 off, u32 val, u32 usec_wait)
{
	if (tg3_flag(tp, PCIX_TARGET_HWBUG) || tg3_flag(tp, ICH_WORKAROUND))
		/* Non-posted methods */
		tp->write32(tp, off, val);
	else {
		/* Posted method */
		tg3_write32(tp, off, val);
		if (usec_wait)
			udelay(usec_wait);
		tp->read32(tp, off);
	}
	/* Wait again after the read for the posted method to guarantee that
	 * the wait time is met.
	 */
	if (usec_wait)
		udelay(usec_wait);
}
