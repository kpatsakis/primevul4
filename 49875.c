int hash_resume_state(struct hash_device_data *device_data,
		      const struct hash_state *device_state)
{
	u32 temp_cr;
	s32 count;
	int hash_mode = HASH_OPER_MODE_HASH;

	if (NULL == device_state) {
		dev_err(device_data->dev, "%s: HASH_INVALID_PARAMETER!\n",
			__func__);
		return -EPERM;
	}

	/* Check correctness of index and length members */
	if (device_state->index > HASH_BLOCK_SIZE ||
	    (device_state->length.low_word % HASH_BLOCK_SIZE) != 0) {
		dev_err(device_data->dev, "%s: HASH_INVALID_PARAMETER!\n",
			__func__);
		return -EPERM;
	}

	/*
	 * INIT bit. Set this bit to 0b1 to reset the HASH processor core and
	 * prepare the initialize the HASH accelerator to compute the message
	 * digest of a new message.
	 */
	HASH_INITIALIZE;

	temp_cr = device_state->temp_cr;
	writel_relaxed(temp_cr & HASH_CR_RESUME_MASK, &device_data->base->cr);

	if (readl(&device_data->base->cr) & HASH_CR_MODE_MASK)
		hash_mode = HASH_OPER_MODE_HMAC;
	else
		hash_mode = HASH_OPER_MODE_HASH;

	for (count = 0; count < HASH_CSR_COUNT; count++) {
		if ((count >= 36) && (hash_mode == HASH_OPER_MODE_HASH))
			break;

		writel_relaxed(device_state->csr[count],
			       &device_data->base->csrx[count]);
	}

	writel_relaxed(device_state->csfull, &device_data->base->csfull);
	writel_relaxed(device_state->csdatain, &device_data->base->csdatain);

	writel_relaxed(device_state->str_reg, &device_data->base->str);
	writel_relaxed(temp_cr, &device_data->base->cr);

	return 0;
}
