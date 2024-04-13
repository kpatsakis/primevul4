int hash_check_hw(struct hash_device_data *device_data)
{
	/* Checking Peripheral Ids  */
	if (HASH_P_ID0 == readl_relaxed(&device_data->base->periphid0) &&
	    HASH_P_ID1 == readl_relaxed(&device_data->base->periphid1) &&
	    HASH_P_ID2 == readl_relaxed(&device_data->base->periphid2) &&
	    HASH_P_ID3 == readl_relaxed(&device_data->base->periphid3) &&
	    HASH_CELL_ID0 == readl_relaxed(&device_data->base->cellid0) &&
	    HASH_CELL_ID1 == readl_relaxed(&device_data->base->cellid1) &&
	    HASH_CELL_ID2 == readl_relaxed(&device_data->base->cellid2) &&
	    HASH_CELL_ID3 == readl_relaxed(&device_data->base->cellid3)) {
		return 0;
	}

	dev_err(device_data->dev, "%s: HASH_UNSUPPORTED_HW!\n", __func__);
	return -ENOTSUPP;
}
