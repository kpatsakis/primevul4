static inline int __ap_query_configuration(struct ap_config_info *config)
{
	register unsigned long reg0 asm ("0") = 0x04000000UL;
	register unsigned long reg1 asm ("1") = -EINVAL;
	register unsigned char *reg2 asm ("2") = (unsigned char *)config;

	asm volatile(
		".long 0xb2af0000\n"		/* PQAP(QCI) */
		"0: la    %1,0\n"
		"1:\n"
		EX_TABLE(0b, 1b)
		: "+d" (reg0), "+d" (reg1), "+d" (reg2)
		:
		: "cc");

	return reg1;
}
