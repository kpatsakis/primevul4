static int adf_ctl_ioctl_dev_stop(struct file *fp, unsigned int cmd,
				  unsigned long arg)
{
	int ret;
	struct adf_user_cfg_ctl_data *ctl_data;

	ret = adf_ctl_alloc_resources(&ctl_data, arg);
	if (ret)
		return ret;

	if (adf_devmgr_verify_id(ctl_data->device_id)) {
		pr_err("QAT: Device %d not found\n", ctl_data->device_id);
		ret = -ENODEV;
		goto out;
	}

	ret = adf_ctl_is_device_in_use(ctl_data->device_id);
	if (ret)
		goto out;

	if (ctl_data->device_id == ADF_CFG_ALL_DEVICES)
		pr_info("QAT: Stopping all acceleration devices.\n");
	else
		pr_info("QAT: Stopping acceleration device qat_dev%d.\n",
			ctl_data->device_id);

	ret = adf_ctl_stop_devices(ctl_data->device_id);
	if (ret)
		pr_err("QAT: failed to stop device.\n");
out:
	kfree(ctl_data);
	return ret;
}
