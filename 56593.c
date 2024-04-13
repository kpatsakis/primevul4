static int handle_vmfunc(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	struct vmcs12 *vmcs12;
	u32 function = vcpu->arch.regs[VCPU_REGS_RAX];

	/*
	 * VMFUNC is only supported for nested guests, but we always enable the
	 * secondary control for simplicity; for non-nested mode, fake that we
	 * didn't by injecting #UD.
	 */
	if (!is_guest_mode(vcpu)) {
		kvm_queue_exception(vcpu, UD_VECTOR);
		return 1;
	}

	vmcs12 = get_vmcs12(vcpu);
	if ((vmcs12->vm_function_control & (1 << function)) == 0)
		goto fail;

	switch (function) {
	case 0:
		if (nested_vmx_eptp_switching(vcpu, vmcs12))
			goto fail;
		break;
	default:
		goto fail;
	}
	return kvm_skip_emulated_instruction(vcpu);

fail:
	nested_vmx_vmexit(vcpu, vmx->exit_reason,
			  vmcs_read32(VM_EXIT_INTR_INFO),
			  vmcs_readl(EXIT_QUALIFICATION));
	return 1;
}
