void *hashbin_remove_first( hashbin_t *hashbin)
{
	unsigned long flags = 0;
	irda_queue_t *entry = NULL;

	/* Synchronize */
	if ( hashbin->hb_type & HB_LOCK ) {
		spin_lock_irqsave(&hashbin->hb_spinlock, flags);
	} /* Default is no-lock  */

	entry = hashbin_get_first( hashbin);
	if ( entry != NULL) {
		int	bin;
		long	hashv;
		/*
		 * Locate hashbin
		 */
		hashv = entry->q_hash;
		bin = GET_HASHBIN( hashv );

		/*
		 * Dequeue the entry...
		 */
		dequeue_general( (irda_queue_t**) &hashbin->hb_queue[ bin ],
				 entry);
		hashbin->hb_size--;
		entry->q_next = NULL;
		entry->q_prev = NULL;

		/*
		 *  Check if this item is the currently selected item, and in
		 *  that case we must reset hb_current
		 */
		if ( entry == hashbin->hb_current)
			hashbin->hb_current = NULL;
	}

	/* Release lock */
	if ( hashbin->hb_type & HB_LOCK ) {
		spin_unlock_irqrestore(&hashbin->hb_spinlock, flags);
	} /* Default is no-lock  */

	return entry;
}
