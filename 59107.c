void* hashbin_remove( hashbin_t* hashbin, long hashv, const char* name)
{
	int bin, found = FALSE;
	unsigned long flags = 0;
	irda_queue_t* entry;

	IRDA_ASSERT( hashbin != NULL, return NULL;);
	IRDA_ASSERT( hashbin->magic == HB_MAGIC, return NULL;);

	/*
	 * Locate hashbin
	 */
	if ( name )
		hashv = hash( name );
	bin = GET_HASHBIN( hashv );

	/* Synchronize */
	if ( hashbin->hb_type & HB_LOCK ) {
		spin_lock_irqsave(&hashbin->hb_spinlock, flags);
	} /* Default is no-lock  */

	/*
	 * Search for entry
	 */
	entry = hashbin->hb_queue[ bin ];
	if ( entry ) {
		do {
			/*
			 * Check for key
			 */
			if ( entry->q_hash == hashv ) {
				/*
				 * Name compare too?
				 */
				if ( name ) {
					if ( strcmp( entry->q_name, name) == 0)
					{
						found = TRUE;
						break;
					}
				} else {
					found = TRUE;
					break;
				}
			}
			entry = entry->q_next;
		} while ( entry != hashbin->hb_queue[ bin ] );
	}

	/*
	 * If entry was found, dequeue it
	 */
	if ( found ) {
		dequeue_general( (irda_queue_t**) &hashbin->hb_queue[ bin ],
				 entry);
		hashbin->hb_size--;

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


	/* Return */
	if ( found )
		return entry;
	else
		return NULL;

}
