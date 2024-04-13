static int iscsi_check_boolean_value(struct iscsi_param *param, char *value)
{
	if (strcmp(value, YES) && strcmp(value, NO)) {
		pr_err("Illegal value for \"%s\", must be either"
			" \"%s\" or \"%s\".\n", param->name, YES, NO);
		return -1;
	}

	return 0;
}
