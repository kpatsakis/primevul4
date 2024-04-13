static bool nested_vmx_exit_handled_cr(struct kvm_vcpu *vcpu,
	struct vmcs12 *vmcs12)
{
	unsigned long exit_qualification = vmcs_readl(EXIT_QUALIFICATION);
	int cr = exit_qualification & 15;
	int reg = (exit_qualification >> 8) & 15;
	unsigned long val = kvm_register_read(vcpu, reg);

	switch ((exit_qualification >> 4) & 3) {
	case 0: /* mov to cr */
		switch (cr) {
		case 0:
			if (vmcs12->cr0_guest_host_mask &
			    (val ^ vmcs12->cr0_read_shadow))
				return 1;
			break;
		case 3:
			if ((vmcs12->cr3_target_count >= 1 &&
					vmcs12->cr3_target_value0 == val) ||
				(vmcs12->cr3_target_count >= 2 &&
					vmcs12->cr3_target_value1 == val) ||
				(vmcs12->cr3_target_count >= 3 &&
					vmcs12->cr3_target_value2 == val) ||
				(vmcs12->cr3_target_count >= 4 &&
					vmcs12->cr3_target_value3 == val))
				return 0;
			if (nested_cpu_has(vmcs12, CPU_BASED_CR3_LOAD_EXITING))
				return 1;
			break;
		case 4:
			if (vmcs12->cr4_guest_host_mask &
			    (vmcs12->cr4_read_shadow ^ val))
				return 1;
			break;
		case 8:
			if (nested_cpu_has(vmcs12, CPU_BASED_CR8_LOAD_EXITING))
				return 1;
			break;
		}
		break;
	case 2: /* clts */
		if ((vmcs12->cr0_guest_host_mask & X86_CR0_TS) &&
		    (vmcs12->cr0_read_shadow & X86_CR0_TS))
			return 1;
		break;
	case 1: /* mov from cr */
		switch (cr) {
		case 3:
			if (vmcs12->cpu_based_vm_exec_control &
			    CPU_BASED_CR3_STORE_EXITING)
				return 1;
			break;
		case 8:
			if (vmcs12->cpu_based_vm_exec_control &
			    CPU_BASED_CR8_STORE_EXITING)
				return 1;
			break;
		}
		break;
	case 3: /* lmsw */
		/*
		 * lmsw can change bits 1..3 of cr0, and only set bit 0 of
		 * cr0. Other attempted changes are ignored, with no exit.
		 */
		if (vmcs12->cr0_guest_host_mask & 0xe &
		    (val ^ vmcs12->cr0_read_shadow))
			return 1;
		if ((vmcs12->cr0_guest_host_mask & 0x1) &&
		    !(vmcs12->cr0_read_shadow & 0x1) &&
		    (val & 0x1))
			return 1;
		break;
	}
	return 0;
}
