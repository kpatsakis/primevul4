static int enable_irq_window(struct kvm_vcpu *vcpu)
{
	u32 cpu_based_vm_exec_control;

	if (is_guest_mode(vcpu) && nested_exit_on_intr(vcpu))
		/*
		 * We get here if vmx_interrupt_allowed() said we can't
		 * inject to L1 now because L2 must run. The caller will have
		 * to make L2 exit right after entry, so we can inject to L1
		 * more promptly.
		 */
		return -EBUSY;

	cpu_based_vm_exec_control = vmcs_read32(CPU_BASED_VM_EXEC_CONTROL);
	cpu_based_vm_exec_control |= CPU_BASED_VIRTUAL_INTR_PENDING;
	vmcs_write32(CPU_BASED_VM_EXEC_CONTROL, cpu_based_vm_exec_control);
	return 0;
}
