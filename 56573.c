static void enable_irq_window(struct kvm_vcpu *vcpu)
{
	vmcs_set_bits(CPU_BASED_VM_EXEC_CONTROL,
		      CPU_BASED_VIRTUAL_INTR_PENDING);
}
