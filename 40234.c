static int iscsi_check_acceptor_state(struct iscsi_param *param, char *value,
				struct iscsi_conn *conn)
{
	u8 acceptor_boolean_value = 0, proposer_boolean_value = 0;
	char *negoitated_value = NULL;

	if (IS_PSTATE_ACCEPTOR(param)) {
		pr_err("Received key \"%s\" twice, protocol error.\n",
				param->name);
		return -1;
	}

	if (IS_PSTATE_REJECT(param))
		return 0;

	if (IS_TYPE_BOOL_AND(param)) {
		if (!strcmp(value, YES))
			proposer_boolean_value = 1;
		if (!strcmp(param->value, YES))
			acceptor_boolean_value = 1;
		if (acceptor_boolean_value && proposer_boolean_value)
			do {} while (0);
		else {
			if (iscsi_update_param_value(param, NO) < 0)
				return -1;
			if (!proposer_boolean_value)
				SET_PSTATE_REPLY_OPTIONAL(param);
		}
	} else if (IS_TYPE_BOOL_OR(param)) {
		if (!strcmp(value, YES))
			proposer_boolean_value = 1;
		if (!strcmp(param->value, YES))
			acceptor_boolean_value = 1;
		if (acceptor_boolean_value || proposer_boolean_value) {
			if (iscsi_update_param_value(param, YES) < 0)
				return -1;
			if (proposer_boolean_value)
				SET_PSTATE_REPLY_OPTIONAL(param);
		}
	} else if (IS_TYPE_NUMBER(param)) {
		char *tmpptr, buf[11];
		u32 acceptor_value = simple_strtoul(param->value, &tmpptr, 0);
		u32 proposer_value = simple_strtoul(value, &tmpptr, 0);

		memset(buf, 0, sizeof(buf));

		if (!strcmp(param->name, MAXCONNECTIONS) ||
		    !strcmp(param->name, MAXBURSTLENGTH) ||
		    !strcmp(param->name, FIRSTBURSTLENGTH) ||
		    !strcmp(param->name, MAXOUTSTANDINGR2T) ||
		    !strcmp(param->name, DEFAULTTIME2RETAIN) ||
		    !strcmp(param->name, ERRORRECOVERYLEVEL)) {
			if (proposer_value > acceptor_value) {
				sprintf(buf, "%u", acceptor_value);
				if (iscsi_update_param_value(param,
						&buf[0]) < 0)
					return -1;
			} else {
				if (iscsi_update_param_value(param, value) < 0)
					return -1;
			}
		} else if (!strcmp(param->name, DEFAULTTIME2WAIT)) {
			if (acceptor_value > proposer_value) {
				sprintf(buf, "%u", acceptor_value);
				if (iscsi_update_param_value(param,
						&buf[0]) < 0)
					return -1;
			} else {
				if (iscsi_update_param_value(param, value) < 0)
					return -1;
			}
		} else {
			if (iscsi_update_param_value(param, value) < 0)
				return -1;
		}

		if (!strcmp(param->name, MAXRECVDATASEGMENTLENGTH)) {
			struct iscsi_param *param_mxdsl;
			unsigned long long tmp;
			int rc;

			rc = strict_strtoull(param->value, 0, &tmp);
			if (rc < 0)
				return -1;

			conn->conn_ops->MaxRecvDataSegmentLength = tmp;
			pr_debug("Saving op->MaxRecvDataSegmentLength from"
				" original initiator received value: %u\n",
				conn->conn_ops->MaxRecvDataSegmentLength);

			param_mxdsl = iscsi_find_param_from_key(
						MAXXMITDATASEGMENTLENGTH,
						conn->param_list);
			if (!param_mxdsl)
				return -1;

			rc = iscsi_update_param_value(param,
						param_mxdsl->value);
			if (rc < 0)
				return -1;

			pr_debug("Updated %s to target MXDSL value: %s\n",
					param->name, param->value);
		}

	} else if (IS_TYPE_NUMBER_RANGE(param)) {
		negoitated_value = iscsi_get_value_from_number_range(
					param, value);
		if (!negoitated_value)
			return -1;
		if (iscsi_update_param_value(param, negoitated_value) < 0)
			return -1;
	} else if (IS_TYPE_VALUE_LIST(param)) {
		negoitated_value = iscsi_check_valuelist_for_support(
					param, value);
		if (!negoitated_value) {
			pr_err("Proposer's value list \"%s\" contains"
				" no valid values from Acceptor's value list"
				" \"%s\".\n", value, param->value);
			return -1;
		}
		if (iscsi_update_param_value(param, negoitated_value) < 0)
			return -1;
	} else if (IS_PHASE_DECLARATIVE(param)) {
		if (iscsi_update_param_value(param, value) < 0)
			return -1;
		SET_PSTATE_REPLY_OPTIONAL(param);
	}

	return 0;
}
