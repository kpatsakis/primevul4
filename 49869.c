static void hash_hw_write_key(struct hash_device_data *device_data,
			      const u8 *key, unsigned int keylen)
{
	u32 word = 0;
	int nwords = 1;

	HASH_CLEAR_BITS(&device_data->base->str, HASH_STR_NBLW_MASK);

	while (keylen >= 4) {
		u32 *key_word = (u32 *)key;

		HASH_SET_DIN(key_word, nwords);
		keylen -= 4;
		key += 4;
	}

	/* Take care of the remaining bytes in the last word */
	if (keylen) {
		word = 0;
		while (keylen) {
			word |= (key[keylen - 1] << (8 * (keylen - 1)));
			keylen--;
		}

		HASH_SET_DIN(&word, nwords);
	}

	while (readl(&device_data->base->str) & HASH_STR_DCAL_MASK)
		cpu_relax();

	HASH_SET_DCAL;

	while (readl(&device_data->base->str) & HASH_STR_DCAL_MASK)
		cpu_relax();
}
