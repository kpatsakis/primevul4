static int ap_interrupts_available(void)
{
	return test_facility(2) && test_facility(65);
}
