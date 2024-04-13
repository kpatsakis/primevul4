static int handle_ept_misconfig(struct kvm_vcpu *vcpu)
{
	u64 sptes[4];
	int nr_sptes, i, ret;
	gpa_t gpa;

	gpa = vmcs_read64(GUEST_PHYSICAL_ADDRESS);

	ret = handle_mmio_page_fault_common(vcpu, gpa, true);
	if (likely(ret == RET_MMIO_PF_EMULATE))
		return x86_emulate_instruction(vcpu, gpa, 0, NULL, 0) ==
					      EMULATE_DONE;

	if (unlikely(ret == RET_MMIO_PF_INVALID))
		return kvm_mmu_page_fault(vcpu, gpa, 0, NULL, 0);

	if (unlikely(ret == RET_MMIO_PF_RETRY))
		return 1;

	/* It is the real ept misconfig */
	printk(KERN_ERR "EPT: Misconfiguration.\n");
	printk(KERN_ERR "EPT: GPA: 0x%llx\n", gpa);

	nr_sptes = kvm_mmu_get_spte_hierarchy(vcpu, gpa, sptes);

	for (i = PT64_ROOT_LEVEL; i > PT64_ROOT_LEVEL - nr_sptes; --i)
		ept_misconfig_inspect_spte(vcpu, sptes[i-1], i);

	vcpu->run->exit_reason = KVM_EXIT_UNKNOWN;
	vcpu->run->hw.hardware_exit_reason = EXIT_REASON_EPT_MISCONFIG;

	return 0;
}
