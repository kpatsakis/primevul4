void iscsi_release_param_list(struct iscsi_param_list *param_list)
{
	struct iscsi_param *param, *param_tmp;

	list_for_each_entry_safe(param, param_tmp, &param_list->param_list,
			p_list) {
		list_del(&param->p_list);

		kfree(param->name);
		kfree(param->value);
		kfree(param);
	}

	iscsi_release_extra_responses(param_list);

	kfree(param_list);
}
