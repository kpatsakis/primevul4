static int set_key(struct cryp_device_data *device_data,
		   u32 left_key,
		   u32 right_key,
		   enum cryp_key_reg_index index)
{
	struct cryp_key_value key_value;
	int cryp_error;

	dev_dbg(device_data->dev, "[%s]", __func__);

	key_value.key_value_left = left_key;
	key_value.key_value_right = right_key;

	cryp_error = cryp_configure_key_values(device_data,
					       index,
					       key_value);
	if (cryp_error != 0)
		dev_err(device_data->dev, "[%s]: "
			"cryp_configure_key_values() failed!", __func__);

	return cryp_error;
}
