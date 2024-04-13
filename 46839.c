static int handle_ept_misconfig(struct kvm_vcpu *vcpu)
{
	int ret;
	gpa_t gpa;

	gpa = vmcs_read64(GUEST_PHYSICAL_ADDRESS);
	if (!kvm_io_bus_write(vcpu, KVM_FAST_MMIO_BUS, gpa, 0, NULL)) {
		skip_emulated_instruction(vcpu);
		trace_kvm_fast_mmio(gpa);
		return 1;
	}

	ret = handle_mmio_page_fault(vcpu, gpa, true);
	if (likely(ret == RET_MMIO_PF_EMULATE))
		return x86_emulate_instruction(vcpu, gpa, 0, NULL, 0) ==
					      EMULATE_DONE;

	if (unlikely(ret == RET_MMIO_PF_INVALID))
		return kvm_mmu_page_fault(vcpu, gpa, 0, NULL, 0);

	if (unlikely(ret == RET_MMIO_PF_RETRY))
		return 1;

	/* It is the real ept misconfig */
	WARN_ON(1);

	vcpu->run->exit_reason = KVM_EXIT_UNKNOWN;
	vcpu->run->hw.hardware_exit_reason = EXIT_REASON_EPT_MISCONFIG;

	return 0;
}
