static int nested_vmx_run(struct kvm_vcpu *vcpu, bool launch)
{
	struct vmcs12 *vmcs12;
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	u32 interrupt_shadow = vmx_get_interrupt_shadow(vcpu);
	u32 exit_qual;
	int ret;

	if (!nested_vmx_check_permission(vcpu))
		return 1;

	if (!nested_vmx_check_vmcs12(vcpu))
		goto out;

	vmcs12 = get_vmcs12(vcpu);

	if (enable_shadow_vmcs)
		copy_shadow_to_vmcs12(vmx);

	/*
	 * The nested entry process starts with enforcing various prerequisites
	 * on vmcs12 as required by the Intel SDM, and act appropriately when
	 * they fail: As the SDM explains, some conditions should cause the
	 * instruction to fail, while others will cause the instruction to seem
	 * to succeed, but return an EXIT_REASON_INVALID_STATE.
	 * To speed up the normal (success) code path, we should avoid checking
	 * for misconfigurations which will anyway be caught by the processor
	 * when using the merged vmcs02.
	 */
	if (interrupt_shadow & KVM_X86_SHADOW_INT_MOV_SS) {
		nested_vmx_failValid(vcpu,
				     VMXERR_ENTRY_EVENTS_BLOCKED_BY_MOV_SS);
		goto out;
	}

	if (vmcs12->launch_state == launch) {
		nested_vmx_failValid(vcpu,
			launch ? VMXERR_VMLAUNCH_NONCLEAR_VMCS
			       : VMXERR_VMRESUME_NONLAUNCHED_VMCS);
		goto out;
	}

	ret = check_vmentry_prereqs(vcpu, vmcs12);
	if (ret) {
		nested_vmx_failValid(vcpu, ret);
		goto out;
	}

	/*
	 * After this point, the trap flag no longer triggers a singlestep trap
	 * on the vm entry instructions; don't call kvm_skip_emulated_instruction.
	 * This is not 100% correct; for performance reasons, we delegate most
	 * of the checks on host state to the processor.  If those fail,
	 * the singlestep trap is missed.
	 */
	skip_emulated_instruction(vcpu);

	ret = check_vmentry_postreqs(vcpu, vmcs12, &exit_qual);
	if (ret) {
		nested_vmx_entry_failure(vcpu, vmcs12,
					 EXIT_REASON_INVALID_STATE, exit_qual);
		return 1;
	}

	/*
	 * We're finally done with prerequisite checking, and can start with
	 * the nested entry.
	 */

	ret = enter_vmx_non_root_mode(vcpu, true);
	if (ret)
		return ret;

	if (vmcs12->guest_activity_state == GUEST_ACTIVITY_HLT)
		return kvm_vcpu_halt(vcpu);

	vmx->nested.nested_run_pending = 1;

	return 1;

out:
	return kvm_skip_emulated_instruction(vcpu);
}
