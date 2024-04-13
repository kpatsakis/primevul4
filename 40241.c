static int iscsi_check_proposer_state(struct iscsi_param *param, char *value)
{
	if (IS_PSTATE_RESPONSE_GOT(param)) {
		pr_err("Received key \"%s\" twice, protocol error.\n",
				param->name);
		return -1;
	}

	if (IS_TYPE_NUMBER_RANGE(param)) {
		u32 left_val = 0, right_val = 0, recieved_value = 0;
		char *left_val_ptr = NULL, *right_val_ptr = NULL;
		char *tilde_ptr = NULL;

		if (!strcmp(value, IRRELEVANT) || !strcmp(value, REJECT)) {
			if (iscsi_update_param_value(param, value) < 0)
				return -1;
			return 0;
		}

		tilde_ptr = strchr(value, '~');
		if (tilde_ptr) {
			pr_err("Illegal \"~\" in response for \"%s\".\n",
					param->name);
			return -1;
		}
		tilde_ptr = strchr(param->value, '~');
		if (!tilde_ptr) {
			pr_err("Unable to locate numerical range"
				" indicator \"~\" for \"%s\".\n", param->name);
			return -1;
		}
		*tilde_ptr = '\0';

		left_val_ptr = param->value;
		right_val_ptr = param->value + strlen(left_val_ptr) + 1;
		left_val = simple_strtoul(left_val_ptr, NULL, 0);
		right_val = simple_strtoul(right_val_ptr, NULL, 0);
		recieved_value = simple_strtoul(value, NULL, 0);

		*tilde_ptr = '~';

		if ((recieved_value < left_val) ||
		    (recieved_value > right_val)) {
			pr_err("Illegal response \"%s=%u\", value must"
				" be between %u and %u.\n", param->name,
				recieved_value, left_val, right_val);
			return -1;
		}
	} else if (IS_TYPE_VALUE_LIST(param)) {
		char *comma_ptr = NULL, *tmp_ptr = NULL;

		comma_ptr = strchr(value, ',');
		if (comma_ptr) {
			pr_err("Illegal \",\" in response for \"%s\".\n",
					param->name);
			return -1;
		}

		tmp_ptr = iscsi_check_valuelist_for_support(param, value);
		if (!tmp_ptr)
			return -1;
	}

	if (iscsi_update_param_value(param, value) < 0)
		return -1;

	return 0;
}
