static void iscsi_release_extra_responses(struct iscsi_param_list *param_list)
{
	struct iscsi_extra_response *er, *er_tmp;

	list_for_each_entry_safe(er, er_tmp, &param_list->extra_response_list,
			er_list) {
		list_del(&er->er_list);
		kfree(er);
	}
}
