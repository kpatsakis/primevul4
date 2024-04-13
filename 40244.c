int iscsi_copy_param_list(
	struct iscsi_param_list **dst_param_list,
	struct iscsi_param_list *src_param_list,
	int leading)
{
	struct iscsi_param *param = NULL;
	struct iscsi_param *new_param = NULL;
	struct iscsi_param_list *param_list = NULL;

	param_list = kzalloc(sizeof(struct iscsi_param_list), GFP_KERNEL);
	if (!param_list) {
		pr_err("Unable to allocate memory for struct iscsi_param_list.\n");
		goto err_out;
	}
	INIT_LIST_HEAD(&param_list->param_list);
	INIT_LIST_HEAD(&param_list->extra_response_list);

	list_for_each_entry(param, &src_param_list->param_list, p_list) {
		if (!leading && (param->scope & SCOPE_SESSION_WIDE)) {
			if ((strcmp(param->name, "TargetName") != 0) &&
			    (strcmp(param->name, "InitiatorName") != 0) &&
			    (strcmp(param->name, "TargetPortalGroupTag") != 0))
				continue;
		}

		new_param = kzalloc(sizeof(struct iscsi_param), GFP_KERNEL);
		if (!new_param) {
			pr_err("Unable to allocate memory for struct iscsi_param.\n");
			goto err_out;
		}

		new_param->name = kstrdup(param->name, GFP_KERNEL);
		new_param->value = kstrdup(param->value, GFP_KERNEL);
		if (!new_param->value || !new_param->name) {
			kfree(new_param->value);
			kfree(new_param->name);
			kfree(new_param);
			pr_err("Unable to allocate memory for parameter name/value.\n");
			goto err_out;
		}

		new_param->set_param = param->set_param;
		new_param->phase = param->phase;
		new_param->scope = param->scope;
		new_param->sender = param->sender;
		new_param->type = param->type;
		new_param->use = param->use;
		new_param->type_range = param->type_range;

		list_add_tail(&new_param->p_list, &param_list->param_list);
	}

	if (!list_empty(&param_list->param_list)) {
		*dst_param_list = param_list;
	} else {
		pr_err("No parameters allocated.\n");
		goto err_out;
	}

	return 0;

err_out:
	iscsi_release_param_list(param_list);
	return -1;
}
