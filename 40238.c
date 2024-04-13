int iscsi_check_negotiated_keys(struct iscsi_param_list *param_list)
{
	int ret = 0;
	struct iscsi_param *param;

	list_for_each_entry(param, &param_list->param_list, p_list) {
		if (IS_PSTATE_NEGOTIATE(param) &&
		    IS_PSTATE_PROPOSER(param) &&
		    !IS_PSTATE_RESPONSE_GOT(param) &&
		    !IS_PSTATE_REPLY_OPTIONAL(param) &&
		    !IS_PHASE_DECLARATIVE(param)) {
			pr_err("No response for proposed key \"%s\".\n",
					param->name);
			ret = -1;
		}
	}

	return ret;
}
