static inline void pi_set_sn(struct pi_desc *pi_desc)
{
	return set_bit(POSTED_INTR_SN,
			(unsigned long *)&pi_desc->control);
}
