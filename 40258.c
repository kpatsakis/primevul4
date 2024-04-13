int iscsi_set_keys_to_negotiate(
	struct iscsi_param_list *param_list,
	bool iser)
{
	struct iscsi_param *param;

	param_list->iser = iser;

	list_for_each_entry(param, &param_list->param_list, p_list) {
		param->state = 0;
		if (!strcmp(param->name, AUTHMETHOD)) {
			SET_PSTATE_NEGOTIATE(param);
		} else if (!strcmp(param->name, HEADERDIGEST)) {
			if (iser == false)
				SET_PSTATE_NEGOTIATE(param);
		} else if (!strcmp(param->name, DATADIGEST)) {
			if (iser == false)
				SET_PSTATE_NEGOTIATE(param);
		} else if (!strcmp(param->name, MAXCONNECTIONS)) {
			SET_PSTATE_NEGOTIATE(param);
		} else if (!strcmp(param->name, TARGETNAME)) {
			continue;
		} else if (!strcmp(param->name, INITIATORNAME)) {
			continue;
		} else if (!strcmp(param->name, TARGETALIAS)) {
			if (param->value)
				SET_PSTATE_NEGOTIATE(param);
		} else if (!strcmp(param->name, INITIATORALIAS)) {
			continue;
		} else if (!strcmp(param->name, TARGETPORTALGROUPTAG)) {
			SET_PSTATE_NEGOTIATE(param);
		} else if (!strcmp(param->name, INITIALR2T)) {
			SET_PSTATE_NEGOTIATE(param);
		} else if (!strcmp(param->name, IMMEDIATEDATA)) {
			SET_PSTATE_NEGOTIATE(param);
		} else if (!strcmp(param->name, MAXRECVDATASEGMENTLENGTH)) {
			if (iser == false)
				SET_PSTATE_NEGOTIATE(param);
		} else if (!strcmp(param->name, MAXXMITDATASEGMENTLENGTH)) {
			continue;
		} else if (!strcmp(param->name, MAXBURSTLENGTH)) {
			SET_PSTATE_NEGOTIATE(param);
		} else if (!strcmp(param->name, FIRSTBURSTLENGTH)) {
			SET_PSTATE_NEGOTIATE(param);
		} else if (!strcmp(param->name, DEFAULTTIME2WAIT)) {
			SET_PSTATE_NEGOTIATE(param);
		} else if (!strcmp(param->name, DEFAULTTIME2RETAIN)) {
			SET_PSTATE_NEGOTIATE(param);
		} else if (!strcmp(param->name, MAXOUTSTANDINGR2T)) {
			SET_PSTATE_NEGOTIATE(param);
		} else if (!strcmp(param->name, DATAPDUINORDER)) {
			SET_PSTATE_NEGOTIATE(param);
		} else if (!strcmp(param->name, DATASEQUENCEINORDER)) {
			SET_PSTATE_NEGOTIATE(param);
		} else if (!strcmp(param->name, ERRORRECOVERYLEVEL)) {
			SET_PSTATE_NEGOTIATE(param);
		} else if (!strcmp(param->name, SESSIONTYPE)) {
			SET_PSTATE_NEGOTIATE(param);
		} else if (!strcmp(param->name, IFMARKER)) {
			SET_PSTATE_NEGOTIATE(param);
		} else if (!strcmp(param->name, OFMARKER)) {
			SET_PSTATE_NEGOTIATE(param);
		} else if (!strcmp(param->name, IFMARKINT)) {
			SET_PSTATE_NEGOTIATE(param);
		} else if (!strcmp(param->name, OFMARKINT)) {
			SET_PSTATE_NEGOTIATE(param);
		} else if (!strcmp(param->name, RDMAEXTENSIONS)) {
			if (iser == true)
				SET_PSTATE_NEGOTIATE(param);
		} else if (!strcmp(param->name, INITIATORRECVDATASEGMENTLENGTH)) {
			if (iser == true)
				SET_PSTATE_NEGOTIATE(param);
		} else if (!strcmp(param->name, TARGETRECVDATASEGMENTLENGTH)) {
			if (iser == true)
				SET_PSTATE_NEGOTIATE(param);
		}
	}

	return 0;
}
