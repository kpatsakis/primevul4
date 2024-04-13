static inline void tg3_full_lock(struct tg3 *tp, int irq_sync)
{
	spin_lock_bh(&tp->lock);
	if (irq_sync)
		tg3_irq_quiesce(tp);
}
