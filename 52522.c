static void mct_u232_msr_to_icount(struct async_icount *icount,
						unsigned char msr)
{
	/* Translate Control Line states */
	if (msr & MCT_U232_MSR_DDSR)
		icount->dsr++;
	if (msr & MCT_U232_MSR_DCTS)
		icount->cts++;
	if (msr & MCT_U232_MSR_DRI)
		icount->rng++;
	if (msr & MCT_U232_MSR_DCD)
		icount->dcd++;
} /* mct_u232_msr_to_icount */
