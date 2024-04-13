static int handle_nmi_window(struct kvm_vcpu *vcpu)
{
	vmcs_clear_bits(CPU_BASED_VM_EXEC_CONTROL,
			CPU_BASED_VIRTUAL_NMI_PENDING);
	++vcpu->stat.nmi_window_exits;
	kvm_make_request(KVM_REQ_EVENT, vcpu);

	return 1;
}
