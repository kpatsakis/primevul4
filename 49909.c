static int ap_configuration_available(void)
{
	return test_facility(2) && test_facility(12);
}
