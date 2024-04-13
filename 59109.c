void* hashbin_remove_this( hashbin_t* hashbin, irda_queue_t* entry)
{
	unsigned long flags = 0;
	int	bin;
	long	hashv;

	IRDA_ASSERT( hashbin != NULL, return NULL;);
	IRDA_ASSERT( hashbin->magic == HB_MAGIC, return NULL;);
	IRDA_ASSERT( entry != NULL, return NULL;);

	/* Synchronize */
	if ( hashbin->hb_type & HB_LOCK ) {
		spin_lock_irqsave(&hashbin->hb_spinlock, flags);
	} /* Default is no-lock  */

	/* Check if valid and not already removed... */
	if((entry->q_next == NULL) || (entry->q_prev == NULL)) {
		entry = NULL;
		goto out;
	}

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
out:
	/* Release lock */
	if ( hashbin->hb_type & HB_LOCK ) {
		spin_unlock_irqrestore(&hashbin->hb_spinlock, flags);
	} /* Default is no-lock  */

	return entry;
}
