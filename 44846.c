fst_process_tx_work_q(unsigned long /*void **/work_q)
{
	unsigned long flags;
	u64 work_txq;
	int i;

	/*
	 * Grab the queue exclusively
	 */
	dbg(DBG_TX, "fst_process_tx_work_q\n");
	spin_lock_irqsave(&fst_work_q_lock, flags);
	work_txq = fst_work_txq;
	fst_work_txq = 0;
	spin_unlock_irqrestore(&fst_work_q_lock, flags);

	/*
	 * Call the bottom half for each card with work waiting
	 */
	for (i = 0; i < FST_MAX_CARDS; i++) {
		if (work_txq & 0x01) {
			if (fst_card_array[i] != NULL) {
				dbg(DBG_TX, "Calling tx bh for card %d\n", i);
				do_bottom_half_tx(fst_card_array[i]);
			}
		}
		work_txq = work_txq >> 1;
	}
}
