static void ap_query_configuration(void)
{
#ifdef CONFIG_64BIT
	if (ap_configuration_available()) {
		if (!ap_configuration)
			ap_configuration =
				kzalloc(sizeof(struct ap_config_info),
					GFP_KERNEL);
		if (ap_configuration)
			__ap_query_configuration(ap_configuration);
	} else
		ap_configuration = NULL;
#else
	ap_configuration = NULL;
#endif
}
