static int iscsi_check_numerical_range_value(struct iscsi_param *param, char *value)
{
	char *left_val_ptr = NULL, *right_val_ptr = NULL;
	char *tilde_ptr = NULL;
	u32 left_val, right_val, local_left_val;

	if (strcmp(param->name, IFMARKINT) &&
	    strcmp(param->name, OFMARKINT)) {
		pr_err("Only parameters \"%s\" or \"%s\" may contain a"
		       " numerical range value.\n", IFMARKINT, OFMARKINT);
		return -1;
	}

	if (IS_PSTATE_PROPOSER(param))
		return 0;

	tilde_ptr = strchr(value, '~');
	if (!tilde_ptr) {
		pr_err("Unable to locate numerical range indicator"
			" \"~\" for \"%s\".\n", param->name);
		return -1;
	}
	*tilde_ptr = '\0';

	left_val_ptr = value;
	right_val_ptr = value + strlen(left_val_ptr) + 1;

	if (iscsi_check_numerical_value(param, left_val_ptr) < 0)
		return -1;
	if (iscsi_check_numerical_value(param, right_val_ptr) < 0)
		return -1;

	left_val = simple_strtoul(left_val_ptr, NULL, 0);
	right_val = simple_strtoul(right_val_ptr, NULL, 0);
	*tilde_ptr = '~';

	if (right_val < left_val) {
		pr_err("Numerical range for parameter \"%s\" contains"
			" a right value which is less than the left.\n",
				param->name);
		return -1;
	}

	/*
	 * For now,  enforce reasonable defaults for [I,O]FMarkInt.
	 */
	tilde_ptr = strchr(param->value, '~');
	if (!tilde_ptr) {
		pr_err("Unable to locate numerical range indicator"
			" \"~\" for \"%s\".\n", param->name);
		return -1;
	}
	*tilde_ptr = '\0';

	left_val_ptr = param->value;
	right_val_ptr = param->value + strlen(left_val_ptr) + 1;

	local_left_val = simple_strtoul(left_val_ptr, NULL, 0);
	*tilde_ptr = '~';

	if (param->set_param) {
		if ((left_val < local_left_val) ||
		    (right_val < local_left_val)) {
			pr_err("Passed value range \"%u~%u\" is below"
				" minimum left value \"%u\" for key \"%s\","
				" rejecting.\n", left_val, right_val,
				local_left_val, param->name);
			return -1;
		}
	} else {
		if ((left_val < local_left_val) &&
		    (right_val < local_left_val)) {
			pr_err("Received value range \"%u~%u\" is"
				" below minimum left value \"%u\" for key"
				" \"%s\", rejecting.\n", left_val, right_val,
				local_left_val, param->name);
			SET_PSTATE_REJECT(param);
			if (iscsi_update_param_value(param, REJECT) < 0)
				return -1;
		}
	}

	return 0;
}
