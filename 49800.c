static int adf_copy_key_value_data(struct adf_accel_dev *accel_dev,
				   struct adf_user_cfg_ctl_data *ctl_data)
{
	struct adf_user_cfg_key_val key_val;
	struct adf_user_cfg_key_val *params_head;
	struct adf_user_cfg_section section, *section_head;

	section_head = ctl_data->config_section;

	while (section_head) {
		if (copy_from_user(&section, (void __user *)section_head,
				   sizeof(*section_head))) {
			pr_err("QAT: failed to copy section info\n");
			goto out_err;
		}

		if (adf_cfg_section_add(accel_dev, section.name)) {
			pr_err("QAT: failed to add section.\n");
			goto out_err;
		}

		params_head = section_head->params;

		while (params_head) {
			if (copy_from_user(&key_val, (void __user *)params_head,
					   sizeof(key_val))) {
				pr_err("QAT: Failed to copy keyvalue.\n");
				goto out_err;
			}
			if (adf_add_key_value_data(accel_dev, section.name,
						   &key_val)) {
				goto out_err;
			}
			params_head = key_val.next;
		}
		section_head = section.next;
	}
	return 0;
out_err:
	adf_cfg_del_all(accel_dev);
	return -EFAULT;
}
