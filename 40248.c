static int iscsi_enforce_integrity_rules(
	u8 phase,
	struct iscsi_param_list *param_list)
{
	char *tmpptr;
	u8 DataSequenceInOrder = 0;
	u8 ErrorRecoveryLevel = 0, SessionType = 0;
	u8 IFMarker = 0, OFMarker = 0;
	u8 IFMarkInt_Reject = 1, OFMarkInt_Reject = 1;
	u32 FirstBurstLength = 0, MaxBurstLength = 0;
	struct iscsi_param *param = NULL;

	list_for_each_entry(param, &param_list->param_list, p_list) {
		if (!(param->phase & phase))
			continue;
		if (!strcmp(param->name, SESSIONTYPE))
			if (!strcmp(param->value, NORMAL))
				SessionType = 1;
		if (!strcmp(param->name, ERRORRECOVERYLEVEL))
			ErrorRecoveryLevel = simple_strtoul(param->value,
					&tmpptr, 0);
		if (!strcmp(param->name, DATASEQUENCEINORDER))
			if (!strcmp(param->value, YES))
				DataSequenceInOrder = 1;
		if (!strcmp(param->name, MAXBURSTLENGTH))
			MaxBurstLength = simple_strtoul(param->value,
					&tmpptr, 0);
		if (!strcmp(param->name, IFMARKER))
			if (!strcmp(param->value, YES))
				IFMarker = 1;
		if (!strcmp(param->name, OFMARKER))
			if (!strcmp(param->value, YES))
				OFMarker = 1;
		if (!strcmp(param->name, IFMARKINT))
			if (!strcmp(param->value, REJECT))
				IFMarkInt_Reject = 1;
		if (!strcmp(param->name, OFMARKINT))
			if (!strcmp(param->value, REJECT))
				OFMarkInt_Reject = 1;
	}

	list_for_each_entry(param, &param_list->param_list, p_list) {
		if (!(param->phase & phase))
			continue;
		if (!SessionType && (!IS_PSTATE_ACCEPTOR(param) &&
		     (strcmp(param->name, IFMARKER) &&
		      strcmp(param->name, OFMARKER) &&
		      strcmp(param->name, IFMARKINT) &&
		      strcmp(param->name, OFMARKINT))))
			continue;
		if (!strcmp(param->name, MAXOUTSTANDINGR2T) &&
		    DataSequenceInOrder && (ErrorRecoveryLevel > 0)) {
			if (strcmp(param->value, "1")) {
				if (iscsi_update_param_value(param, "1") < 0)
					return -1;
				pr_debug("Reset \"%s\" to \"%s\".\n",
					param->name, param->value);
			}
		}
		if (!strcmp(param->name, MAXCONNECTIONS) && !SessionType) {
			if (strcmp(param->value, "1")) {
				if (iscsi_update_param_value(param, "1") < 0)
					return -1;
				pr_debug("Reset \"%s\" to \"%s\".\n",
					param->name, param->value);
			}
		}
		if (!strcmp(param->name, FIRSTBURSTLENGTH)) {
			FirstBurstLength = simple_strtoul(param->value,
					&tmpptr, 0);
			if (FirstBurstLength > MaxBurstLength) {
				char tmpbuf[11];
				memset(tmpbuf, 0, sizeof(tmpbuf));
				sprintf(tmpbuf, "%u", MaxBurstLength);
				if (iscsi_update_param_value(param, tmpbuf))
					return -1;
				pr_debug("Reset \"%s\" to \"%s\".\n",
					param->name, param->value);
			}
		}
		if (!strcmp(param->name, IFMARKER) && IFMarkInt_Reject) {
			if (iscsi_update_param_value(param, NO) < 0)
				return -1;
			IFMarker = 0;
			pr_debug("Reset \"%s\" to \"%s\".\n",
					param->name, param->value);
		}
		if (!strcmp(param->name, OFMARKER) && OFMarkInt_Reject) {
			if (iscsi_update_param_value(param, NO) < 0)
				return -1;
			OFMarker = 0;
			pr_debug("Reset \"%s\" to \"%s\".\n",
					 param->name, param->value);
		}
		if (!strcmp(param->name, IFMARKINT) && !IFMarker) {
			if (!strcmp(param->value, REJECT))
				continue;
			param->state &= ~PSTATE_NEGOTIATE;
			if (iscsi_update_param_value(param, IRRELEVANT) < 0)
				return -1;
			pr_debug("Reset \"%s\" to \"%s\".\n",
					param->name, param->value);
		}
		if (!strcmp(param->name, OFMARKINT) && !OFMarker) {
			if (!strcmp(param->value, REJECT))
				continue;
			param->state &= ~PSTATE_NEGOTIATE;
			if (iscsi_update_param_value(param, IRRELEVANT) < 0)
				return -1;
			pr_debug("Reset \"%s\" to \"%s\".\n",
					param->name, param->value);
		}
	}

	return 0;
}
