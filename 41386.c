static u32 tg3_irq_count(struct tg3 *tp)
{
	u32 irq_cnt = max(tp->rxq_cnt, tp->txq_cnt);

	if (irq_cnt > 1) {
		/* We want as many rx rings enabled as there are cpus.
		 * In multiqueue MSI-X mode, the first MSI-X vector
		 * only deals with link interrupts, etc, so we add
		 * one to the number of vectors we are requesting.
		 */
		irq_cnt = min_t(unsigned, irq_cnt + 1, tp->irq_max);
	}

	return irq_cnt;
}
