static int handle_interrupt_window(struct kvm_vcpu *vcpu)
{
	vmcs_clear_bits(CPU_BASED_VM_EXEC_CONTROL,
			CPU_BASED_VIRTUAL_INTR_PENDING);

	kvm_make_request(KVM_REQ_EVENT, vcpu);

	++vcpu->stat.irq_window_exits;
	return 1;
}
