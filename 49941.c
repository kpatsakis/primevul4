static int ap_query_functions(ap_qid_t qid, unsigned int *functions)
{
#ifdef CONFIG_64BIT
	struct ap_queue_status status;
	int i;
	status = __ap_query_functions(qid, functions);

	for (i = 0; i < AP_MAX_RESET; i++) {
		if (ap_queue_status_invalid_test(&status))
			return -ENODEV;

		switch (status.response_code) {
		case AP_RESPONSE_NORMAL:
			return 0;
		case AP_RESPONSE_RESET_IN_PROGRESS:
		case AP_RESPONSE_BUSY:
			break;
		case AP_RESPONSE_Q_NOT_AVAIL:
		case AP_RESPONSE_DECONFIGURED:
		case AP_RESPONSE_CHECKSTOPPED:
		case AP_RESPONSE_INVALID_ADDRESS:
			return -ENODEV;
		case AP_RESPONSE_OTHERWISE_CHANGED:
			break;
		default:
			break;
		}
		if (i < AP_MAX_RESET - 1) {
			udelay(5);
			status = __ap_query_functions(qid, functions);
		}
	}
	return -EBUSY;
#else
	return -EINVAL;
#endif
}
