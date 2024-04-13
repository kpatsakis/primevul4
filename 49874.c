static void hash_processblock(struct hash_device_data *device_data,
			      const u32 *message, int length)
{
	int len = length / HASH_BYTES_PER_WORD;
	/*
	 * NBLW bits. Reset the number of bits in last word (NBLW).
	 */
	HASH_CLEAR_BITS(&device_data->base->str, HASH_STR_NBLW_MASK);

	/*
	 * Write message data to the HASH_DIN register.
	 */
	HASH_SET_DIN(message, len);
}
