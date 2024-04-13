ap_queue_interruption_control(ap_qid_t qid, void *ind)
{
	register unsigned long reg0 asm ("0") = qid | 0x03000000UL;
	register unsigned long reg1_in asm ("1") = 0x0000800000000000UL | AP_ISC;
	register struct ap_queue_status reg1_out asm ("1");
	register void *reg2 asm ("2") = ind;
	asm volatile(
		".long 0xb2af0000"		/* PQAP(AQIC) */
		: "+d" (reg0), "+d" (reg1_in), "=d" (reg1_out), "+d" (reg2)
		:
		: "cc" );
	return reg1_out;
}
