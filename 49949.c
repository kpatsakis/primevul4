static void ap_reset_all(void)
{
	int i, j;

	for (i = 0; i < AP_DOMAINS; i++) {
		if (!ap_test_config_domain(i))
			continue;
		for (j = 0; j < AP_DEVICES; j++) {
			if (!ap_test_config_card_id(j))
				continue;
			ap_reset_queue(AP_MKQID(j, i));
		}
	}
}
