int hash_save_state(struct hash_device_data *device_data,
		    struct hash_state *device_state)
{
	u32 temp_cr;
	u32 count;
	int hash_mode = HASH_OPER_MODE_HASH;

	if (NULL == device_state) {
		dev_err(device_data->dev, "%s: HASH_INVALID_PARAMETER!\n",
			__func__);
		return -ENOTSUPP;
	}

	/* Write dummy value to force digest intermediate calculation. This
	 * actually makes sure that there isn't any ongoing calculation in the
	 * hardware.
	 */
	while (readl(&device_data->base->str) & HASH_STR_DCAL_MASK)
		cpu_relax();

	temp_cr = readl_relaxed(&device_data->base->cr);

	device_state->str_reg = readl_relaxed(&device_data->base->str);

	device_state->din_reg = readl_relaxed(&device_data->base->din);

	if (readl(&device_data->base->cr) & HASH_CR_MODE_MASK)
		hash_mode = HASH_OPER_MODE_HMAC;
	else
		hash_mode = HASH_OPER_MODE_HASH;

	for (count = 0; count < HASH_CSR_COUNT; count++) {
		if ((count >= 36) && (hash_mode == HASH_OPER_MODE_HASH))
			break;

		device_state->csr[count] =
			readl_relaxed(&device_data->base->csrx[count]);
	}

	device_state->csfull = readl_relaxed(&device_data->base->csfull);
	device_state->csdatain = readl_relaxed(&device_data->base->csdatain);

	device_state->temp_cr = temp_cr;

	return 0;
}
