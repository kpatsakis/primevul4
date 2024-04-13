static void hash_messagepad(struct hash_device_data *device_data,
			    const u32 *message, u8 index_bytes)
{
	int nwords = 1;

	/*
	 * Clear hash str register, only clear NBLW
	 * since DCAL will be reset by hardware.
	 */
	HASH_CLEAR_BITS(&device_data->base->str, HASH_STR_NBLW_MASK);

	/* Main loop */
	while (index_bytes >= 4) {
		HASH_SET_DIN(message, nwords);
		index_bytes -= 4;
		message++;
	}

	if (index_bytes)
		HASH_SET_DIN(message, nwords);

	while (readl(&device_data->base->str) & HASH_STR_DCAL_MASK)
		cpu_relax();

	/* num_of_bytes == 0 => NBLW <- 0 (32 bits valid in DATAIN) */
	HASH_SET_NBLW(index_bytes * 8);
	dev_dbg(device_data->dev, "%s: DIN=0x%08x NBLW=%lu\n",
		__func__, readl_relaxed(&device_data->base->din),
		readl_relaxed(&device_data->base->str) & HASH_STR_NBLW_MASK);
	HASH_SET_DCAL;
	dev_dbg(device_data->dev, "%s: after dcal -> DIN=0x%08x NBLW=%lu\n",
		__func__, readl_relaxed(&device_data->base->din),
		readl_relaxed(&device_data->base->str) & HASH_STR_NBLW_MASK);

	while (readl(&device_data->base->str) & HASH_STR_DCAL_MASK)
		cpu_relax();
}
