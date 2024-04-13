static int handle_vmwrite(struct kvm_vcpu *vcpu)
{
	unsigned long field;
	gva_t gva;
	unsigned long exit_qualification = vmcs_readl(EXIT_QUALIFICATION);
	u32 vmx_instruction_info = vmcs_read32(VMX_INSTRUCTION_INFO);
	/* The value to write might be 32 or 64 bits, depending on L1's long
	 * mode, and eventually we need to write that into a field of several
	 * possible lengths. The code below first zero-extends the value to 64
	 * bit (field_value), and then copies only the approriate number of
	 * bits into the vmcs12 field.
	 */
	u64 field_value = 0;
	struct x86_exception e;

	if (!nested_vmx_check_permission(vcpu) ||
	    !nested_vmx_check_vmcs12(vcpu))
		return 1;

	if (vmx_instruction_info & (1u << 10))
		field_value = kvm_register_readl(vcpu,
			(((vmx_instruction_info) >> 3) & 0xf));
	else {
		if (get_vmx_mem_address(vcpu, exit_qualification,
				vmx_instruction_info, false, &gva))
			return 1;
		if (kvm_read_guest_virt(&vcpu->arch.emulate_ctxt, gva,
			   &field_value, (is_64_bit_mode(vcpu) ? 8 : 4), &e)) {
			kvm_inject_page_fault(vcpu, &e);
			return 1;
		}
	}


	field = kvm_register_readl(vcpu, (((vmx_instruction_info) >> 28) & 0xf));
	if (vmcs_field_readonly(field)) {
		nested_vmx_failValid(vcpu,
			VMXERR_VMWRITE_READ_ONLY_VMCS_COMPONENT);
		skip_emulated_instruction(vcpu);
		return 1;
	}

	if (vmcs12_write_any(vcpu, field, field_value) < 0) {
		nested_vmx_failValid(vcpu, VMXERR_UNSUPPORTED_VMCS_COMPONENT);
		skip_emulated_instruction(vcpu);
		return 1;
	}

	nested_vmx_succeed(vcpu);
	skip_emulated_instruction(vcpu);
	return 1;
}
