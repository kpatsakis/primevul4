irda_queue_t *hashbin_get_next( hashbin_t *hashbin)
{
	irda_queue_t* entry;
	int bin;
	int i;

	IRDA_ASSERT( hashbin != NULL, return NULL;);
	IRDA_ASSERT( hashbin->magic == HB_MAGIC, return NULL;);

	if ( hashbin->hb_current == NULL) {
		IRDA_ASSERT( hashbin->hb_current != NULL, return NULL;);
		return NULL;
	}
	entry = hashbin->hb_current->q_next;
	bin = GET_HASHBIN( entry->q_hash);

	/*
	 *  Make sure that we are not back at the beginning of the queue
	 *  again
	 */
	if ( entry != hashbin->hb_queue[ bin ]) {
		hashbin->hb_current = entry;

		return entry;
	}

	/*
	 *  Check that this is not the last queue in hashbin
	 */
	if ( bin >= HASHBIN_SIZE)
		return NULL;

	/*
	 *  Move to next queue in hashbin
	 */
	bin++;
	for ( i = bin; i < HASHBIN_SIZE; i++ ) {
		entry = hashbin->hb_queue[ i];
		if ( entry) {
			hashbin->hb_current = entry;

			return entry;
		}
	}
	return NULL;
}
