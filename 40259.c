int iscsi_update_param_value(struct iscsi_param *param, char *value)
{
	kfree(param->value);

	param->value = kstrdup(value, GFP_KERNEL);
	if (!param->value) {
		pr_err("Unable to allocate memory for value.\n");
		return -ENOMEM;
	}

	pr_debug("iSCSI Parameter updated to %s=%s\n",
			param->name, param->value);
	return 0;
}
