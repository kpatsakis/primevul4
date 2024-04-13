static void kvm_init_msr_list(void)
{
	u32 dummy[2];
	unsigned i, j;

	/* skip the first msrs in the list. KVM-specific */
	for (i = j = KVM_SAVE_MSRS_BEGIN; i < ARRAY_SIZE(msrs_to_save); i++) {
		if (rdmsr_safe(msrs_to_save[i], &dummy[0], &dummy[1]) < 0)
			continue;

		/*
		 * Even MSRs that are valid in the host may not be exposed
		 * to the guests in some cases.  We could work around this
		 * in VMX with the generic MSR save/load machinery, but it
		 * is not really worthwhile since it will really only
		 * happen with nested virtualization.
		 */
		switch (msrs_to_save[i]) {
		case MSR_IA32_BNDCFGS:
			if (!kvm_x86_ops->mpx_supported())
				continue;
			break;
		default:
			break;
		}

		if (j < i)
			msrs_to_save[j] = msrs_to_save[i];
		j++;
	}
	num_msrs_to_save = j;
}
