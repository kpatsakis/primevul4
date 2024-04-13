static inline int ap_test_config_domain(unsigned int domain)
{
	if (!ap_configuration)
		return 1;
	return ap_test_config(ap_configuration->aqm, domain);
}
