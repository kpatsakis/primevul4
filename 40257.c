int iscsi_set_keys_irrelevant_for_discovery(
	struct iscsi_param_list *param_list)
{
	struct iscsi_param *param;

	list_for_each_entry(param, &param_list->param_list, p_list) {
		if (!strcmp(param->name, MAXCONNECTIONS))
			param->state &= ~PSTATE_NEGOTIATE;
		else if (!strcmp(param->name, INITIALR2T))
			param->state &= ~PSTATE_NEGOTIATE;
		else if (!strcmp(param->name, IMMEDIATEDATA))
			param->state &= ~PSTATE_NEGOTIATE;
		else if (!strcmp(param->name, MAXBURSTLENGTH))
			param->state &= ~PSTATE_NEGOTIATE;
		else if (!strcmp(param->name, FIRSTBURSTLENGTH))
			param->state &= ~PSTATE_NEGOTIATE;
		else if (!strcmp(param->name, MAXOUTSTANDINGR2T))
			param->state &= ~PSTATE_NEGOTIATE;
		else if (!strcmp(param->name, DATAPDUINORDER))
			param->state &= ~PSTATE_NEGOTIATE;
		else if (!strcmp(param->name, DATASEQUENCEINORDER))
			param->state &= ~PSTATE_NEGOTIATE;
		else if (!strcmp(param->name, ERRORRECOVERYLEVEL))
			param->state &= ~PSTATE_NEGOTIATE;
		else if (!strcmp(param->name, DEFAULTTIME2WAIT))
			param->state &= ~PSTATE_NEGOTIATE;
		else if (!strcmp(param->name, DEFAULTTIME2RETAIN))
			param->state &= ~PSTATE_NEGOTIATE;
		else if (!strcmp(param->name, IFMARKER))
			param->state &= ~PSTATE_NEGOTIATE;
		else if (!strcmp(param->name, OFMARKER))
			param->state &= ~PSTATE_NEGOTIATE;
		else if (!strcmp(param->name, IFMARKINT))
			param->state &= ~PSTATE_NEGOTIATE;
		else if (!strcmp(param->name, OFMARKINT))
			param->state &= ~PSTATE_NEGOTIATE;
		else if (!strcmp(param->name, RDMAEXTENSIONS))
			param->state &= ~PSTATE_NEGOTIATE;
		else if (!strcmp(param->name, INITIATORRECVDATASEGMENTLENGTH))
			param->state &= ~PSTATE_NEGOTIATE;
		else if (!strcmp(param->name, TARGETRECVDATASEGMENTLENGTH))
			param->state &= ~PSTATE_NEGOTIATE;
	}

	return 0;
}
