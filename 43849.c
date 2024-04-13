static void ept_misconfig_inspect_spte(struct kvm_vcpu *vcpu, u64 spte,
				       int level)
{
	printk(KERN_ERR "%s: spte 0x%llx level %d\n", __func__, spte, level);

	/* 010b (write-only) */
	WARN_ON((spte & 0x7) == 0x2);

	/* 110b (write/execute) */
	WARN_ON((spte & 0x7) == 0x6);

	/* 100b (execute-only) and value not supported by logical processor */
	if (!cpu_has_vmx_ept_execute_only())
		WARN_ON((spte & 0x7) == 0x4);

	/* not 000b */
	if ((spte & 0x7)) {
		u64 rsvd_bits = spte & ept_rsvd_mask(spte, level);

		if (rsvd_bits != 0) {
			printk(KERN_ERR "%s: rsvd_bits = 0x%llx\n",
					 __func__, rsvd_bits);
			WARN_ON(1);
		}

		if (level == 1 || (level == 2 && (spte & (1ULL << 7)))) {
			u64 ept_mem_type = (spte & 0x38) >> 3;

			if (ept_mem_type == 2 || ept_mem_type == 3 ||
			    ept_mem_type == 7) {
				printk(KERN_ERR "%s: ept_mem_type=0x%llx\n",
						__func__, ept_mem_type);
				WARN_ON(1);
			}
		}
	}
}
