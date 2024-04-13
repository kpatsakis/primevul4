void hash_begin(struct hash_device_data *device_data, struct hash_ctx *ctx)
{
	/* HW and SW initializations */
	/* Note: there is no need to initialize buffer and digest members */

	while (readl(&device_data->base->str) & HASH_STR_DCAL_MASK)
		cpu_relax();

	/*
	 * INIT bit. Set this bit to 0b1 to reset the HASH processor core and
	 * prepare the initialize the HASH accelerator to compute the message
	 * digest of a new message.
	 */
	HASH_INITIALIZE;

	/*
	 * NBLW bits. Reset the number of bits in last word (NBLW).
	 */
	HASH_CLEAR_BITS(&device_data->base->str, HASH_STR_NBLW_MASK);
}
