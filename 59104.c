irda_queue_t *hashbin_get_first( hashbin_t* hashbin)
{
	irda_queue_t *entry;
	int i;

	IRDA_ASSERT( hashbin != NULL, return NULL;);
	IRDA_ASSERT( hashbin->magic == HB_MAGIC, return NULL;);

	if ( hashbin == NULL)
		return NULL;

	for ( i = 0; i < HASHBIN_SIZE; i ++ ) {
		entry = hashbin->hb_queue[ i];
		if ( entry) {
			hashbin->hb_current = entry;
			return entry;
		}
	}
	/*
	 *  Did not find any item in hashbin
	 */
	return NULL;
}
