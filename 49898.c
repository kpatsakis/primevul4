__ap_query_functions(ap_qid_t qid, unsigned int *functions)
{
	register unsigned long reg0 asm ("0") = 0UL | qid | (1UL << 23);
	register struct ap_queue_status reg1 asm ("1") = AP_QUEUE_STATUS_INVALID;
	register unsigned long reg2 asm ("2");

	asm volatile(
		".long 0xb2af0000\n"		/* PQAP(TAPQ) */
		"0:\n"
		EX_TABLE(0b, 0b)
		: "+d" (reg0), "+d" (reg1), "=d" (reg2)
		:
		: "cc");

	*functions = (unsigned int)(reg2 >> 32);
	return reg1;
}
