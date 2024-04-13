int iscsi_change_param_value(
	char *keyvalue,
	struct iscsi_param_list *param_list,
	int check_key)
{
	char *key = NULL, *value = NULL;
	struct iscsi_param *param;
	int sender = 0;

	if (iscsi_extract_key_value(keyvalue, &key, &value) < 0)
		return -1;

	if (!check_key) {
		param = __iscsi_check_key(keyvalue, sender, param_list);
		if (!param)
			return -1;
	} else {
		param = iscsi_check_key(keyvalue, 0, sender, param_list);
		if (!param)
			return -1;

		param->set_param = 1;
		if (iscsi_check_value(param, value) < 0) {
			param->set_param = 0;
			return -1;
		}
		param->set_param = 0;
	}

	if (iscsi_update_param_value(param, value) < 0)
		return -1;

	return 0;
}
