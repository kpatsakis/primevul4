static int apply_microcode_early(struct ucode_cpu_info *uci, bool early)
{
	struct microcode_intel *mc_intel;
	unsigned int val[2];

	mc_intel = uci->mc;
	if (mc_intel == NULL)
		return 0;

	/* write microcode via MSR 0x79 */
	native_wrmsr(MSR_IA32_UCODE_WRITE,
	      (unsigned long) mc_intel->bits,
	      (unsigned long) mc_intel->bits >> 16 >> 16);
	native_wrmsr(MSR_IA32_UCODE_REV, 0, 0);

	/* As documented in the SDM: Do a CPUID 1 here */
	sync_core();

	/* get the current revision from MSR 0x8B */
	native_rdmsr(MSR_IA32_UCODE_REV, val[0], val[1]);
	if (val[1] != mc_intel->hdr.rev)
		return -1;

#ifdef CONFIG_X86_64
	/* Flush global tlb. This is precaution. */
	flush_tlb_early();
#endif
	uci->cpu_sig.rev = val[1];

	if (early)
		print_ucode(uci);
	else
		print_ucode_info(uci, mc_intel->hdr.date);

	return 0;
}
