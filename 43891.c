static int vmx_handle_exit(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	u32 exit_reason = vmx->exit_reason;
	u32 vectoring_info = vmx->idt_vectoring_info;

	/* If guest state is invalid, start emulating */
	if (vmx->emulation_required)
		return handle_invalid_guest_state(vcpu);

	/*
	 * the KVM_REQ_EVENT optimization bit is only on for one entry, and if
	 * we did not inject a still-pending event to L1 now because of
	 * nested_run_pending, we need to re-enable this bit.
	 */
	if (vmx->nested.nested_run_pending)
		kvm_make_request(KVM_REQ_EVENT, vcpu);

	if (!is_guest_mode(vcpu) && (exit_reason == EXIT_REASON_VMLAUNCH ||
	    exit_reason == EXIT_REASON_VMRESUME))
		vmx->nested.nested_run_pending = 1;
	else
		vmx->nested.nested_run_pending = 0;

	if (is_guest_mode(vcpu) && nested_vmx_exit_handled(vcpu)) {
		nested_vmx_vmexit(vcpu);
		return 1;
	}

	if (exit_reason & VMX_EXIT_REASONS_FAILED_VMENTRY) {
		vcpu->run->exit_reason = KVM_EXIT_FAIL_ENTRY;
		vcpu->run->fail_entry.hardware_entry_failure_reason
			= exit_reason;
		return 0;
	}

	if (unlikely(vmx->fail)) {
		vcpu->run->exit_reason = KVM_EXIT_FAIL_ENTRY;
		vcpu->run->fail_entry.hardware_entry_failure_reason
			= vmcs_read32(VM_INSTRUCTION_ERROR);
		return 0;
	}

	/*
	 * Note:
	 * Do not try to fix EXIT_REASON_EPT_MISCONFIG if it caused by
	 * delivery event since it indicates guest is accessing MMIO.
	 * The vm-exit can be triggered again after return to guest that
	 * will cause infinite loop.
	 */
	if ((vectoring_info & VECTORING_INFO_VALID_MASK) &&
			(exit_reason != EXIT_REASON_EXCEPTION_NMI &&
			exit_reason != EXIT_REASON_EPT_VIOLATION &&
			exit_reason != EXIT_REASON_TASK_SWITCH)) {
		vcpu->run->exit_reason = KVM_EXIT_INTERNAL_ERROR;
		vcpu->run->internal.suberror = KVM_INTERNAL_ERROR_DELIVERY_EV;
		vcpu->run->internal.ndata = 2;
		vcpu->run->internal.data[0] = vectoring_info;
		vcpu->run->internal.data[1] = exit_reason;
		return 0;
	}

	if (unlikely(!cpu_has_virtual_nmis() && vmx->soft_vnmi_blocked &&
	    !(is_guest_mode(vcpu) && nested_cpu_has_virtual_nmis(
	                                get_vmcs12(vcpu), vcpu)))) {
		if (vmx_interrupt_allowed(vcpu)) {
			vmx->soft_vnmi_blocked = 0;
		} else if (vmx->vnmi_blocked_time > 1000000000LL &&
			   vcpu->arch.nmi_pending) {
			/*
			 * This CPU don't support us in finding the end of an
			 * NMI-blocked window if the guest runs with IRQs
			 * disabled. So we pull the trigger after 1 s of
			 * futile waiting, but inform the user about this.
			 */
			printk(KERN_WARNING "%s: Breaking out of NMI-blocked "
			       "state on VCPU %d after 1 s timeout\n",
			       __func__, vcpu->vcpu_id);
			vmx->soft_vnmi_blocked = 0;
		}
	}

	if (exit_reason < kvm_vmx_max_exit_handlers
	    && kvm_vmx_exit_handlers[exit_reason])
		return kvm_vmx_exit_handlers[exit_reason](vcpu);
	else {
		vcpu->run->exit_reason = KVM_EXIT_UNKNOWN;
		vcpu->run->hw.hardware_exit_reason = exit_reason;
	}
	return 0;
}
