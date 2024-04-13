static int adf_ctl_stop_devices(uint32_t id)
{
	struct list_head *itr, *head = adf_devmgr_get_head();
	int ret = 0;

	list_for_each(itr, head) {
		struct adf_accel_dev *accel_dev =
				list_entry(itr, struct adf_accel_dev, list);
		if (id == accel_dev->accel_id || id == ADF_CFG_ALL_DEVICES) {
			if (!adf_dev_started(accel_dev))
				continue;

			if (adf_dev_stop(accel_dev)) {
				pr_err("QAT: Failed to stop qat_dev%d\n", id);
				ret = -EFAULT;
			}
		}
	}
	return ret;
}
