static inline bool nested_cpu_has_virtual_nmis(struct vmcs12 *vmcs12,
	struct kvm_vcpu *vcpu)
{
	return vmcs12->pin_based_vm_exec_control & PIN_BASED_VIRTUAL_NMIS;
}
