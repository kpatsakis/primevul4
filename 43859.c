static int handle_invalid_guest_state(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	enum emulation_result err = EMULATE_DONE;
	int ret = 1;
	u32 cpu_exec_ctrl;
	bool intr_window_requested;
	unsigned count = 130;

	cpu_exec_ctrl = vmcs_read32(CPU_BASED_VM_EXEC_CONTROL);
	intr_window_requested = cpu_exec_ctrl & CPU_BASED_VIRTUAL_INTR_PENDING;

	while (!guest_state_valid(vcpu) && count-- != 0) {
		if (intr_window_requested && vmx_interrupt_allowed(vcpu))
			return handle_interrupt_window(&vmx->vcpu);

		if (test_bit(KVM_REQ_EVENT, &vcpu->requests))
			return 1;

		err = emulate_instruction(vcpu, EMULTYPE_NO_REEXECUTE);

		if (err == EMULATE_USER_EXIT) {
			ret = 0;
			goto out;
		}

		if (err != EMULATE_DONE) {
			vcpu->run->exit_reason = KVM_EXIT_INTERNAL_ERROR;
			vcpu->run->internal.suberror = KVM_INTERNAL_ERROR_EMULATION;
			vcpu->run->internal.ndata = 0;
			return 0;
		}

		if (vcpu->arch.halt_request) {
			vcpu->arch.halt_request = 0;
			ret = kvm_emulate_halt(vcpu);
			goto out;
		}

		if (signal_pending(current))
			goto out;
		if (need_resched())
			schedule();
	}

	vmx->emulation_required = emulation_required(vcpu);
out:
	return ret;
}
