static int nested_vmx_get_vmptr(struct kvm_vcpu *vcpu, gpa_t *vmpointer)
{
	gva_t gva;
	struct x86_exception e;

	if (get_vmx_mem_address(vcpu, vmcs_readl(EXIT_QUALIFICATION),
			vmcs_read32(VMX_INSTRUCTION_INFO), false, &gva))
		return 1;

	if (kvm_read_guest_virt(&vcpu->arch.emulate_ctxt, gva, vmpointer,
				sizeof(*vmpointer), &e)) {
		kvm_inject_page_fault(vcpu, &e);
		return 1;
	}

	return 0;
}
