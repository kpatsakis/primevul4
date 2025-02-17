fst_q_work_item(u64 * queue, int card_index)
{
	unsigned long flags;
	u64 mask;

	/*
	 * Grab the queue exclusively
	 */
	spin_lock_irqsave(&fst_work_q_lock, flags);

	/*
	 * Making an entry in the queue is simply a matter of setting
	 * a bit for the card indicating that there is work to do in the
	 * bottom half for the card.  Note the limitation of 64 cards.
	 * That ought to be enough
	 */
	mask = (u64)1 << card_index;
	*queue |= mask;
	spin_unlock_irqrestore(&fst_work_q_lock, flags);
}
