static inline void tw32_mailbox_flush(struct tg3 *tp, u32 off, u32 val)
{
	tp->write32_mbox(tp, off, val);
	if (tg3_flag(tp, FLUSH_POSTED_WRITES) ||
	    (!tg3_flag(tp, MBOX_WRITE_REORDER) &&
	     !tg3_flag(tp, ICH_WORKAROUND)))
		tp->read32_mbox(tp, off);
}
