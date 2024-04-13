static inline u64 sp_to_dp(u32 fprs)
{
	u64 fprd;

	preempt_disable();
	enable_kernel_fp();
	asm ("lfs%U1%X1 0,%1; stfd%U0%X0 0,%0" : "=m" (fprd) : "m" (fprs)
	     : "fr0");
	preempt_enable();
	return fprd;
}
