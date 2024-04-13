static int adf_add_key_value_data(struct adf_accel_dev *accel_dev,
				  const char *section,
				  const struct adf_user_cfg_key_val *key_val)
{
	if (key_val->type == ADF_HEX) {
		long *ptr = (long *)key_val->val;
		long val = *ptr;

		if (adf_cfg_add_key_value_param(accel_dev, section,
						key_val->key, (void *)val,
						key_val->type)) {
			pr_err("QAT: failed to add keyvalue.\n");
			return -EFAULT;
		}
	} else {
		if (adf_cfg_add_key_value_param(accel_dev, section,
						key_val->key, key_val->val,
						key_val->type)) {
			pr_err("QAT: failed to add keyvalue.\n");
			return -EFAULT;
		}
	}
	return 0;
}
