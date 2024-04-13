static inline int ap_test_config_card_id(unsigned int id)
{
	if (!ap_configuration)
		return 1;
	return ap_test_config(ap_configuration->apm, id);
}
