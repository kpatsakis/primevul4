static int adf_ctl_alloc_resources(struct adf_user_cfg_ctl_data **ctl_data,
				   unsigned long arg)
{
	struct adf_user_cfg_ctl_data *cfg_data;

	cfg_data = kzalloc(sizeof(*cfg_data), GFP_KERNEL);
	if (!cfg_data)
		return -ENOMEM;

	/* Initialize device id to NO DEVICE as 0 is a valid device id */
	cfg_data->device_id = ADF_CFG_NO_DEVICE;

	if (copy_from_user(cfg_data, (void __user *)arg, sizeof(*cfg_data))) {
		pr_err("QAT: failed to copy from user cfg_data.\n");
		kfree(cfg_data);
		return -EIO;
	}

	*ctl_data = cfg_data;
	return 0;
}
