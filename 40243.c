static int iscsi_check_value(struct iscsi_param *param, char *value)
{
	char *comma_ptr = NULL;

	if (!strcmp(value, REJECT)) {
		if (!strcmp(param->name, IFMARKINT) ||
		    !strcmp(param->name, OFMARKINT)) {
			/*
			 * Reject is not fatal for [I,O]FMarkInt,  and causes
			 * [I,O]FMarker to be reset to No. (See iSCSI v20 A.3.2)
			 */
			SET_PSTATE_REJECT(param);
			return 0;
		}
		pr_err("Received %s=%s\n", param->name, value);
		return -1;
	}
	if (!strcmp(value, IRRELEVANT)) {
		pr_debug("Received %s=%s\n", param->name, value);
		SET_PSTATE_IRRELEVANT(param);
		return 0;
	}
	if (!strcmp(value, NOTUNDERSTOOD)) {
		if (!IS_PSTATE_PROPOSER(param)) {
			pr_err("Received illegal offer %s=%s\n",
				param->name, value);
			return -1;
		}

/* #warning FIXME: Add check for X-ExtensionKey here */
		pr_err("Standard iSCSI key \"%s\" cannot be answered"
			" with \"%s\", protocol error.\n", param->name, value);
		return -1;
	}

	do {
		comma_ptr = NULL;
		comma_ptr = strchr(value, ',');

		if (comma_ptr && !IS_TYPE_VALUE_LIST(param)) {
			pr_err("Detected value separator \",\", but"
				" key \"%s\" does not allow a value list,"
				" protocol error.\n", param->name);
			return -1;
		}
		if (comma_ptr)
			*comma_ptr = '\0';

		if (strlen(value) > VALUE_MAXLEN) {
			pr_err("Value for key \"%s\" exceeds %d,"
				" protocol error.\n", param->name,
				VALUE_MAXLEN);
			return -1;
		}

		if (IS_TYPE_BOOL_AND(param) || IS_TYPE_BOOL_OR(param)) {
			if (iscsi_check_boolean_value(param, value) < 0)
				return -1;
		} else if (IS_TYPE_NUMBER(param)) {
			if (iscsi_check_numerical_value(param, value) < 0)
				return -1;
		} else if (IS_TYPE_NUMBER_RANGE(param)) {
			if (iscsi_check_numerical_range_value(param, value) < 0)
				return -1;
		} else if (IS_TYPE_STRING(param) || IS_TYPE_VALUE_LIST(param)) {
			if (iscsi_check_string_or_list_value(param, value) < 0)
				return -1;
		} else {
			pr_err("Huh? 0x%02x\n", param->type);
			return -1;
		}

		if (comma_ptr)
			*comma_ptr++ = ',';

		value = comma_ptr;
	} while (value);

	return 0;
}
