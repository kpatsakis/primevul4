static void ap_reset_domain(void)
{
	int i;

	if (ap_domain_index != -1)
		for (i = 0; i < AP_DEVICES; i++)
			ap_reset_queue(AP_MKQID(i, ap_domain_index));
}
