int iscsi_create_default_params(struct iscsi_param_list **param_list_ptr)
{
	struct iscsi_param *param = NULL;
	struct iscsi_param_list *pl;

	pl = kzalloc(sizeof(struct iscsi_param_list), GFP_KERNEL);
	if (!pl) {
		pr_err("Unable to allocate memory for"
				" struct iscsi_param_list.\n");
		return -1 ;
	}
	INIT_LIST_HEAD(&pl->param_list);
	INIT_LIST_HEAD(&pl->extra_response_list);

	/*
	 * The format for setting the initial parameter definitions are:
	 *
	 * Parameter name:
	 * Initial value:
	 * Allowable phase:
	 * Scope:
	 * Allowable senders:
	 * Typerange:
	 * Use:
	 */
	param = iscsi_set_default_param(pl, AUTHMETHOD, INITIAL_AUTHMETHOD,
			PHASE_SECURITY, SCOPE_CONNECTION_ONLY, SENDER_BOTH,
			TYPERANGE_AUTH, USE_INITIAL_ONLY);
	if (!param)
		goto out;

	param = iscsi_set_default_param(pl, HEADERDIGEST, INITIAL_HEADERDIGEST,
			PHASE_OPERATIONAL, SCOPE_CONNECTION_ONLY, SENDER_BOTH,
			TYPERANGE_DIGEST, USE_INITIAL_ONLY);
	if (!param)
		goto out;

	param = iscsi_set_default_param(pl, DATADIGEST, INITIAL_DATADIGEST,
			PHASE_OPERATIONAL, SCOPE_CONNECTION_ONLY, SENDER_BOTH,
			TYPERANGE_DIGEST, USE_INITIAL_ONLY);
	if (!param)
		goto out;

	param = iscsi_set_default_param(pl, MAXCONNECTIONS,
			INITIAL_MAXCONNECTIONS, PHASE_OPERATIONAL,
			SCOPE_SESSION_WIDE, SENDER_BOTH,
			TYPERANGE_1_TO_65535, USE_LEADING_ONLY);
	if (!param)
		goto out;

	param = iscsi_set_default_param(pl, SENDTARGETS, INITIAL_SENDTARGETS,
			PHASE_FFP0, SCOPE_SESSION_WIDE, SENDER_INITIATOR,
			TYPERANGE_UTF8, 0);
	if (!param)
		goto out;

	param = iscsi_set_default_param(pl, TARGETNAME, INITIAL_TARGETNAME,
			PHASE_DECLARATIVE, SCOPE_SESSION_WIDE, SENDER_BOTH,
			TYPERANGE_ISCSINAME, USE_ALL);
	if (!param)
		goto out;

	param = iscsi_set_default_param(pl, INITIATORNAME,
			INITIAL_INITIATORNAME, PHASE_DECLARATIVE,
			SCOPE_SESSION_WIDE, SENDER_INITIATOR,
			TYPERANGE_ISCSINAME, USE_INITIAL_ONLY);
	if (!param)
		goto out;

	param = iscsi_set_default_param(pl, TARGETALIAS, INITIAL_TARGETALIAS,
			PHASE_DECLARATIVE, SCOPE_SESSION_WIDE, SENDER_TARGET,
			TYPERANGE_UTF8, USE_ALL);
	if (!param)
		goto out;

	param = iscsi_set_default_param(pl, INITIATORALIAS,
			INITIAL_INITIATORALIAS, PHASE_DECLARATIVE,
			SCOPE_SESSION_WIDE, SENDER_INITIATOR, TYPERANGE_UTF8,
			USE_ALL);
	if (!param)
		goto out;

	param = iscsi_set_default_param(pl, TARGETADDRESS,
			INITIAL_TARGETADDRESS, PHASE_DECLARATIVE,
			SCOPE_SESSION_WIDE, SENDER_TARGET,
			TYPERANGE_TARGETADDRESS, USE_ALL);
	if (!param)
		goto out;

	param = iscsi_set_default_param(pl, TARGETPORTALGROUPTAG,
			INITIAL_TARGETPORTALGROUPTAG,
			PHASE_DECLARATIVE, SCOPE_SESSION_WIDE, SENDER_TARGET,
			TYPERANGE_0_TO_65535, USE_INITIAL_ONLY);
	if (!param)
		goto out;

	param = iscsi_set_default_param(pl, INITIALR2T, INITIAL_INITIALR2T,
			PHASE_OPERATIONAL, SCOPE_SESSION_WIDE, SENDER_BOTH,
			TYPERANGE_BOOL_OR, USE_LEADING_ONLY);
	if (!param)
		goto out;

	param = iscsi_set_default_param(pl, IMMEDIATEDATA,
			INITIAL_IMMEDIATEDATA, PHASE_OPERATIONAL,
			SCOPE_SESSION_WIDE, SENDER_BOTH, TYPERANGE_BOOL_AND,
			USE_LEADING_ONLY);
	if (!param)
		goto out;

	param = iscsi_set_default_param(pl, MAXXMITDATASEGMENTLENGTH,
			INITIAL_MAXXMITDATASEGMENTLENGTH,
			PHASE_OPERATIONAL, SCOPE_CONNECTION_ONLY, SENDER_BOTH,
			TYPERANGE_512_TO_16777215, USE_ALL);
	if (!param)
		goto out;

	param = iscsi_set_default_param(pl, MAXRECVDATASEGMENTLENGTH,
			INITIAL_MAXRECVDATASEGMENTLENGTH,
			PHASE_OPERATIONAL, SCOPE_CONNECTION_ONLY, SENDER_BOTH,
			TYPERANGE_512_TO_16777215, USE_ALL);
	if (!param)
		goto out;

	param = iscsi_set_default_param(pl, MAXBURSTLENGTH,
			INITIAL_MAXBURSTLENGTH, PHASE_OPERATIONAL,
			SCOPE_SESSION_WIDE, SENDER_BOTH,
			TYPERANGE_512_TO_16777215, USE_LEADING_ONLY);
	if (!param)
		goto out;

	param = iscsi_set_default_param(pl, FIRSTBURSTLENGTH,
			INITIAL_FIRSTBURSTLENGTH,
			PHASE_OPERATIONAL, SCOPE_SESSION_WIDE, SENDER_BOTH,
			TYPERANGE_512_TO_16777215, USE_LEADING_ONLY);
	if (!param)
		goto out;

	param = iscsi_set_default_param(pl, DEFAULTTIME2WAIT,
			INITIAL_DEFAULTTIME2WAIT,
			PHASE_OPERATIONAL, SCOPE_SESSION_WIDE, SENDER_BOTH,
			TYPERANGE_0_TO_3600, USE_LEADING_ONLY);
	if (!param)
		goto out;

	param = iscsi_set_default_param(pl, DEFAULTTIME2RETAIN,
			INITIAL_DEFAULTTIME2RETAIN,
			PHASE_OPERATIONAL, SCOPE_SESSION_WIDE, SENDER_BOTH,
			TYPERANGE_0_TO_3600, USE_LEADING_ONLY);
	if (!param)
		goto out;

	param = iscsi_set_default_param(pl, MAXOUTSTANDINGR2T,
			INITIAL_MAXOUTSTANDINGR2T,
			PHASE_OPERATIONAL, SCOPE_SESSION_WIDE, SENDER_BOTH,
			TYPERANGE_1_TO_65535, USE_LEADING_ONLY);
	if (!param)
		goto out;

	param = iscsi_set_default_param(pl, DATAPDUINORDER,
			INITIAL_DATAPDUINORDER, PHASE_OPERATIONAL,
			SCOPE_SESSION_WIDE, SENDER_BOTH, TYPERANGE_BOOL_OR,
			USE_LEADING_ONLY);
	if (!param)
		goto out;

	param = iscsi_set_default_param(pl, DATASEQUENCEINORDER,
			INITIAL_DATASEQUENCEINORDER,
			PHASE_OPERATIONAL, SCOPE_SESSION_WIDE, SENDER_BOTH,
			TYPERANGE_BOOL_OR, USE_LEADING_ONLY);
	if (!param)
		goto out;

	param = iscsi_set_default_param(pl, ERRORRECOVERYLEVEL,
			INITIAL_ERRORRECOVERYLEVEL,
			PHASE_OPERATIONAL, SCOPE_SESSION_WIDE, SENDER_BOTH,
			TYPERANGE_0_TO_2, USE_LEADING_ONLY);
	if (!param)
		goto out;

	param = iscsi_set_default_param(pl, SESSIONTYPE, INITIAL_SESSIONTYPE,
			PHASE_DECLARATIVE, SCOPE_SESSION_WIDE, SENDER_INITIATOR,
			TYPERANGE_SESSIONTYPE, USE_LEADING_ONLY);
	if (!param)
		goto out;

	param = iscsi_set_default_param(pl, IFMARKER, INITIAL_IFMARKER,
			PHASE_OPERATIONAL, SCOPE_CONNECTION_ONLY, SENDER_BOTH,
			TYPERANGE_BOOL_AND, USE_INITIAL_ONLY);
	if (!param)
		goto out;

	param = iscsi_set_default_param(pl, OFMARKER, INITIAL_OFMARKER,
			PHASE_OPERATIONAL, SCOPE_CONNECTION_ONLY, SENDER_BOTH,
			TYPERANGE_BOOL_AND, USE_INITIAL_ONLY);
	if (!param)
		goto out;

	param = iscsi_set_default_param(pl, IFMARKINT, INITIAL_IFMARKINT,
			PHASE_OPERATIONAL, SCOPE_CONNECTION_ONLY, SENDER_BOTH,
			TYPERANGE_MARKINT, USE_INITIAL_ONLY);
	if (!param)
		goto out;

	param = iscsi_set_default_param(pl, OFMARKINT, INITIAL_OFMARKINT,
			PHASE_OPERATIONAL, SCOPE_CONNECTION_ONLY, SENDER_BOTH,
			TYPERANGE_MARKINT, USE_INITIAL_ONLY);
	if (!param)
		goto out;
	/*
	 * Extra parameters for ISER from RFC-5046
	 */
	param = iscsi_set_default_param(pl, RDMAEXTENSIONS, INITIAL_RDMAEXTENSIONS,
			PHASE_OPERATIONAL, SCOPE_SESSION_WIDE, SENDER_BOTH,
			TYPERANGE_BOOL_AND, USE_LEADING_ONLY);
	if (!param)
		goto out;

	param = iscsi_set_default_param(pl, INITIATORRECVDATASEGMENTLENGTH,
			INITIAL_INITIATORRECVDATASEGMENTLENGTH,
			PHASE_OPERATIONAL, SCOPE_CONNECTION_ONLY, SENDER_BOTH,
			TYPERANGE_512_TO_16777215, USE_ALL);
	if (!param)
		goto out;

	param = iscsi_set_default_param(pl, TARGETRECVDATASEGMENTLENGTH,
			INITIAL_TARGETRECVDATASEGMENTLENGTH,
			PHASE_OPERATIONAL, SCOPE_CONNECTION_ONLY, SENDER_BOTH,
			TYPERANGE_512_TO_16777215, USE_ALL);
	if (!param)
		goto out;

	*param_list_ptr = pl;
	return 0;
out:
	iscsi_release_param_list(pl);
	return -1;
}
