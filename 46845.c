static int handle_invvpid(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	u32 vmx_instruction_info;
	unsigned long type, types;
	gva_t gva;
	struct x86_exception e;
	int vpid;

	if (!(vmx->nested.nested_vmx_secondary_ctls_high &
	      SECONDARY_EXEC_ENABLE_VPID) ||
			!(vmx->nested.nested_vmx_vpid_caps & VMX_VPID_INVVPID_BIT)) {
		kvm_queue_exception(vcpu, UD_VECTOR);
		return 1;
	}

	if (!nested_vmx_check_permission(vcpu))
		return 1;

	vmx_instruction_info = vmcs_read32(VMX_INSTRUCTION_INFO);
	type = kvm_register_readl(vcpu, (vmx_instruction_info >> 28) & 0xf);

	types = (vmx->nested.nested_vmx_vpid_caps >> 8) & 0x7;

	if (!(types & (1UL << type))) {
		nested_vmx_failValid(vcpu,
			VMXERR_INVALID_OPERAND_TO_INVEPT_INVVPID);
		return 1;
	}

	/* according to the intel vmx instruction reference, the memory
	 * operand is read even if it isn't needed (e.g., for type==global)
	 */
	if (get_vmx_mem_address(vcpu, vmcs_readl(EXIT_QUALIFICATION),
			vmx_instruction_info, false, &gva))
		return 1;
	if (kvm_read_guest_virt(&vcpu->arch.emulate_ctxt, gva, &vpid,
				sizeof(u32), &e)) {
		kvm_inject_page_fault(vcpu, &e);
		return 1;
	}

	switch (type) {
	case VMX_VPID_EXTENT_ALL_CONTEXT:
		if (get_vmcs12(vcpu)->virtual_processor_id == 0) {
			nested_vmx_failValid(vcpu,
				VMXERR_INVALID_OPERAND_TO_INVEPT_INVVPID);
			return 1;
		}
		__vmx_flush_tlb(vcpu, to_vmx(vcpu)->nested.vpid02);
		nested_vmx_succeed(vcpu);
		break;
	default:
		/* Trap single context invalidation invvpid calls */
		BUG_ON(1);
		break;
	}

	skip_emulated_instruction(vcpu);
	return 1;
}
