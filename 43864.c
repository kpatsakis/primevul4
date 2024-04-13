static int handle_vmon(struct kvm_vcpu *vcpu)
{
	struct kvm_segment cs;
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	struct vmcs *shadow_vmcs;
	const u64 VMXON_NEEDED_FEATURES = FEATURE_CONTROL_LOCKED
		| FEATURE_CONTROL_VMXON_ENABLED_OUTSIDE_SMX;

	/* The Intel VMX Instruction Reference lists a bunch of bits that
	 * are prerequisite to running VMXON, most notably cr4.VMXE must be
	 * set to 1 (see vmx_set_cr4() for when we allow the guest to set this).
	 * Otherwise, we should fail with #UD. We test these now:
	 */
	if (!kvm_read_cr4_bits(vcpu, X86_CR4_VMXE) ||
	    !kvm_read_cr0_bits(vcpu, X86_CR0_PE) ||
	    (vmx_get_rflags(vcpu) & X86_EFLAGS_VM)) {
		kvm_queue_exception(vcpu, UD_VECTOR);
		return 1;
	}

	vmx_get_segment(vcpu, &cs, VCPU_SREG_CS);
	if (is_long_mode(vcpu) && !cs.l) {
		kvm_queue_exception(vcpu, UD_VECTOR);
		return 1;
	}

	if (vmx_get_cpl(vcpu)) {
		kvm_inject_gp(vcpu, 0);
		return 1;
	}
	if (vmx->nested.vmxon) {
		nested_vmx_failValid(vcpu, VMXERR_VMXON_IN_VMX_ROOT_OPERATION);
		skip_emulated_instruction(vcpu);
		return 1;
	}

	if ((vmx->nested.msr_ia32_feature_control & VMXON_NEEDED_FEATURES)
			!= VMXON_NEEDED_FEATURES) {
		kvm_inject_gp(vcpu, 0);
		return 1;
	}

	if (enable_shadow_vmcs) {
		shadow_vmcs = alloc_vmcs();
		if (!shadow_vmcs)
			return -ENOMEM;
		/* mark vmcs as shadow */
		shadow_vmcs->revision_id |= (1u << 31);
		/* init shadow vmcs */
		vmcs_clear(shadow_vmcs);
		vmx->nested.current_shadow_vmcs = shadow_vmcs;
	}

	INIT_LIST_HEAD(&(vmx->nested.vmcs02_pool));
	vmx->nested.vmcs02_num = 0;

	vmx->nested.vmxon = true;

	skip_emulated_instruction(vcpu);
	nested_vmx_succeed(vcpu);
	return 1;
}
