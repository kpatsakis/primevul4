static struct iscsi_param *__iscsi_check_key(
	char *key,
	int sender,
	struct iscsi_param_list *param_list)
{
	struct iscsi_param *param;

	if (strlen(key) > KEY_MAXLEN) {
		pr_err("Length of key name \"%s\" exceeds %d.\n",
			key, KEY_MAXLEN);
		return NULL;
	}

	param = iscsi_find_param_from_key(key, param_list);
	if (!param)
		return NULL;

	if ((sender & SENDER_INITIATOR) && !IS_SENDER_INITIATOR(param)) {
		pr_err("Key \"%s\" may not be sent to %s,"
			" protocol error.\n", param->name,
			(sender & SENDER_RECEIVER) ? "target" : "initiator");
		return NULL;
	}

	if ((sender & SENDER_TARGET) && !IS_SENDER_TARGET(param)) {
		pr_err("Key \"%s\" may not be sent to %s,"
			" protocol error.\n", param->name,
			(sender & SENDER_RECEIVER) ? "initiator" : "target");
		return NULL;
	}

	return param;
}
