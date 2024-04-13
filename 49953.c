static inline int ap_test_config(unsigned int *field, unsigned int nr)
{
	if (nr > 0xFFu)
		return 0;
	return ap_test_bit((field + (nr >> 5)), (nr & 0x1f));
}
