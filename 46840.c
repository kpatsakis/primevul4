static int handle_ept_violation(struct kvm_vcpu *vcpu)
{
	unsigned long exit_qualification;
	gpa_t gpa;
	u32 error_code;
	int gla_validity;

	exit_qualification = vmcs_readl(EXIT_QUALIFICATION);

	gla_validity = (exit_qualification >> 7) & 0x3;
	if (gla_validity != 0x3 && gla_validity != 0x1 && gla_validity != 0) {
		printk(KERN_ERR "EPT: Handling EPT violation failed!\n");
		printk(KERN_ERR "EPT: GPA: 0x%lx, GVA: 0x%lx\n",
			(long unsigned int)vmcs_read64(GUEST_PHYSICAL_ADDRESS),
			vmcs_readl(GUEST_LINEAR_ADDRESS));
		printk(KERN_ERR "EPT: Exit qualification is 0x%lx\n",
			(long unsigned int)exit_qualification);
		vcpu->run->exit_reason = KVM_EXIT_UNKNOWN;
		vcpu->run->hw.hardware_exit_reason = EXIT_REASON_EPT_VIOLATION;
		return 0;
	}

	/*
	 * EPT violation happened while executing iret from NMI,
	 * "blocked by NMI" bit has to be set before next VM entry.
	 * There are errata that may cause this bit to not be set:
	 * AAK134, BY25.
	 */
	if (!(to_vmx(vcpu)->idt_vectoring_info & VECTORING_INFO_VALID_MASK) &&
			cpu_has_virtual_nmis() &&
			(exit_qualification & INTR_INFO_UNBLOCK_NMI))
		vmcs_set_bits(GUEST_INTERRUPTIBILITY_INFO, GUEST_INTR_STATE_NMI);

	gpa = vmcs_read64(GUEST_PHYSICAL_ADDRESS);
	trace_kvm_page_fault(gpa, exit_qualification);

	/* It is a write fault? */
	error_code = exit_qualification & PFERR_WRITE_MASK;
	/* It is a fetch fault? */
	error_code |= (exit_qualification << 2) & PFERR_FETCH_MASK;
	/* ept page table is present? */
	error_code |= (exit_qualification >> 3) & PFERR_PRESENT_MASK;

	vcpu->arch.exit_qualification = exit_qualification;

	return kvm_mmu_page_fault(vcpu, gpa, error_code, NULL, 0);
}
