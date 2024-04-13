static void iscsi_check_proposer_for_optional_reply(struct iscsi_param *param)
{
	if (IS_TYPE_BOOL_AND(param)) {
		if (!strcmp(param->value, NO))
			SET_PSTATE_REPLY_OPTIONAL(param);
	} else if (IS_TYPE_BOOL_OR(param)) {
		if (!strcmp(param->value, YES))
			SET_PSTATE_REPLY_OPTIONAL(param);
		 /*
		  * Required for gPXE iSCSI boot client
		  */
		if (!strcmp(param->name, IMMEDIATEDATA))
			SET_PSTATE_REPLY_OPTIONAL(param);
	} else if (IS_TYPE_NUMBER(param)) {
		if (!strcmp(param->name, MAXRECVDATASEGMENTLENGTH))
			SET_PSTATE_REPLY_OPTIONAL(param);
		/*
		 * The GlobalSAN iSCSI Initiator for MacOSX does
		 * not respond to MaxBurstLength, FirstBurstLength,
		 * DefaultTime2Wait or DefaultTime2Retain parameter keys.
		 * So, we set them to 'reply optional' here, and assume the
		 * the defaults from iscsi_parameters.h if the initiator
		 * is not RFC compliant and the keys are not negotiated.
		 */
		if (!strcmp(param->name, MAXBURSTLENGTH))
			SET_PSTATE_REPLY_OPTIONAL(param);
		if (!strcmp(param->name, FIRSTBURSTLENGTH))
			SET_PSTATE_REPLY_OPTIONAL(param);
		if (!strcmp(param->name, DEFAULTTIME2WAIT))
			SET_PSTATE_REPLY_OPTIONAL(param);
		if (!strcmp(param->name, DEFAULTTIME2RETAIN))
			SET_PSTATE_REPLY_OPTIONAL(param);
		/*
		 * Required for gPXE iSCSI boot client
		 */
		if (!strcmp(param->name, MAXCONNECTIONS))
			SET_PSTATE_REPLY_OPTIONAL(param);
	} else if (IS_PHASE_DECLARATIVE(param))
		SET_PSTATE_REPLY_OPTIONAL(param);
}
