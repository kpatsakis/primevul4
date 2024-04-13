static struct iscsi_param *iscsi_check_key(
	char *key,
	int phase,
	int sender,
	struct iscsi_param_list *param_list)
{
	struct iscsi_param *param;
	/*
	 * Key name length must not exceed 63 bytes. (See iSCSI v20 5.1)
	 */
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

	if (IS_PSTATE_ACCEPTOR(param)) {
		pr_err("Key \"%s\" received twice, protocol error.\n",
				key);
		return NULL;
	}

	if (!phase)
		return param;

	if (!(param->phase & phase)) {
		pr_err("Key \"%s\" may not be negotiated during ",
				param->name);
		switch (phase) {
		case PHASE_SECURITY:
			pr_debug("Security phase.\n");
			break;
		case PHASE_OPERATIONAL:
			pr_debug("Operational phase.\n");
			break;
		default:
			pr_debug("Unknown phase.\n");
		}
		return NULL;
	}

	return param;
}
