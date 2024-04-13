void* hashbin_find( hashbin_t* hashbin, long hashv, const char* name )
{
	int bin;
	irda_queue_t* entry;

	pr_debug("hashbin_find()\n");

	IRDA_ASSERT( hashbin != NULL, return NULL;);
	IRDA_ASSERT( hashbin->magic == HB_MAGIC, return NULL;);

	/*
	 * Locate hashbin
	 */
	if ( name )
		hashv = hash( name );
	bin = GET_HASHBIN( hashv );

	/*
	 * Search for entry
	 */
	entry = hashbin->hb_queue[ bin];
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
					if ( strcmp( entry->q_name, name ) == 0 ) {
						return entry;
					}
				} else {
					return entry;
				}
			}
			entry = entry->q_next;
		} while ( entry != hashbin->hb_queue[ bin ] );
	}

	return NULL;
}
