static int adf_ctl_is_device_in_use(int id)
{
	struct list_head *itr, *head = adf_devmgr_get_head();

	list_for_each(itr, head) {
		struct adf_accel_dev *dev =
				list_entry(itr, struct adf_accel_dev, list);

		if (id == dev->accel_id || id == ADF_CFG_ALL_DEVICES) {
			if (adf_devmgr_in_reset(dev) || adf_dev_in_use(dev)) {
				pr_info("QAT: device qat_dev%d is busy\n",
					dev->accel_id);
				return -EBUSY;
			}
		}
	}
	return 0;
}
