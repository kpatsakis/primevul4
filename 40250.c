struct iscsi_param *iscsi_find_param_from_key(
	char *key,
	struct iscsi_param_list *param_list)
{
	struct iscsi_param *param;

	if (!key || !param_list) {
		pr_err("Key or parameter list pointer is NULL.\n");
		return NULL;
	}

	list_for_each_entry(param, &param_list->param_list, p_list) {
		if (!strcmp(key, param->name))
			return param;
	}

	pr_err("Unable to locate key \"%s\".\n", key);
	return NULL;
}
