static int adf_ctl_ioctl_get_status(struct file *fp, unsigned int cmd,
				    unsigned long arg)
{
	struct adf_hw_device_data *hw_data;
	struct adf_dev_status_info dev_info;
	struct adf_accel_dev *accel_dev;

	if (copy_from_user(&dev_info, (void __user *)arg,
			   sizeof(struct adf_dev_status_info))) {
		pr_err("QAT: failed to copy from user.\n");
		return -EFAULT;
	}

	accel_dev = adf_devmgr_get_dev_by_id(dev_info.accel_id);
	if (!accel_dev) {
		pr_err("QAT: Device %d not found\n", dev_info.accel_id);
		return -ENODEV;
	}
	hw_data = accel_dev->hw_device;
	dev_info.state = adf_dev_started(accel_dev) ? DEV_UP : DEV_DOWN;
	dev_info.num_ae = hw_data->get_num_aes(hw_data);
	dev_info.num_accel = hw_data->get_num_accels(hw_data);
	dev_info.num_logical_accel = hw_data->num_logical_accel;
	dev_info.banks_per_accel = hw_data->num_banks
					/ hw_data->num_logical_accel;
	strlcpy(dev_info.name, hw_data->dev_class->name, sizeof(dev_info.name));
	dev_info.instance_id = hw_data->instance_id;
	dev_info.type = hw_data->dev_class->type;
	dev_info.bus = accel_to_pci_dev(accel_dev)->bus->number;
	dev_info.dev = PCI_SLOT(accel_to_pci_dev(accel_dev)->devfn);
	dev_info.fun = PCI_FUNC(accel_to_pci_dev(accel_dev)->devfn);

	if (copy_to_user((void __user *)arg, &dev_info,
			 sizeof(struct adf_dev_status_info))) {
		pr_err("QAT: failed to copy status.\n");
		return -EFAULT;
	}
	return 0;
}
