static int vmx_get_vmx_msr(struct kvm_vcpu *vcpu, u32 msr_index, u64 *pdata)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	switch (msr_index) {
	case MSR_IA32_VMX_BASIC:
		/*
		 * This MSR reports some information about VMX support. We
		 * should return information about the VMX we emulate for the
		 * guest, and the VMCS structure we give it - not about the
		 * VMX support of the underlying hardware.
		 */
		*pdata = VMCS12_REVISION | VMX_BASIC_TRUE_CTLS |
			   ((u64)VMCS12_SIZE << VMX_BASIC_VMCS_SIZE_SHIFT) |
			   (VMX_BASIC_MEM_TYPE_WB << VMX_BASIC_MEM_TYPE_SHIFT);
		break;
	case MSR_IA32_VMX_TRUE_PINBASED_CTLS:
	case MSR_IA32_VMX_PINBASED_CTLS:
		*pdata = vmx_control_msr(
			vmx->nested.nested_vmx_pinbased_ctls_low,
			vmx->nested.nested_vmx_pinbased_ctls_high);
		break;
	case MSR_IA32_VMX_TRUE_PROCBASED_CTLS:
		*pdata = vmx_control_msr(
			vmx->nested.nested_vmx_true_procbased_ctls_low,
			vmx->nested.nested_vmx_procbased_ctls_high);
		break;
	case MSR_IA32_VMX_PROCBASED_CTLS:
		*pdata = vmx_control_msr(
			vmx->nested.nested_vmx_procbased_ctls_low,
			vmx->nested.nested_vmx_procbased_ctls_high);
		break;
	case MSR_IA32_VMX_TRUE_EXIT_CTLS:
		*pdata = vmx_control_msr(
			vmx->nested.nested_vmx_true_exit_ctls_low,
			vmx->nested.nested_vmx_exit_ctls_high);
		break;
	case MSR_IA32_VMX_EXIT_CTLS:
		*pdata = vmx_control_msr(
			vmx->nested.nested_vmx_exit_ctls_low,
			vmx->nested.nested_vmx_exit_ctls_high);
		break;
	case MSR_IA32_VMX_TRUE_ENTRY_CTLS:
		*pdata = vmx_control_msr(
			vmx->nested.nested_vmx_true_entry_ctls_low,
			vmx->nested.nested_vmx_entry_ctls_high);
		break;
	case MSR_IA32_VMX_ENTRY_CTLS:
		*pdata = vmx_control_msr(
			vmx->nested.nested_vmx_entry_ctls_low,
			vmx->nested.nested_vmx_entry_ctls_high);
		break;
	case MSR_IA32_VMX_MISC:
		*pdata = vmx_control_msr(
			vmx->nested.nested_vmx_misc_low,
			vmx->nested.nested_vmx_misc_high);
		break;
	/*
	 * These MSRs specify bits which the guest must keep fixed (on or off)
	 * while L1 is in VMXON mode (in L1's root mode, or running an L2).
	 * We picked the standard core2 setting.
	 */
#define VMXON_CR0_ALWAYSON	(X86_CR0_PE | X86_CR0_PG | X86_CR0_NE)
#define VMXON_CR4_ALWAYSON	X86_CR4_VMXE
	case MSR_IA32_VMX_CR0_FIXED0:
		*pdata = VMXON_CR0_ALWAYSON;
		break;
	case MSR_IA32_VMX_CR0_FIXED1:
		*pdata = -1ULL;
		break;
	case MSR_IA32_VMX_CR4_FIXED0:
		*pdata = VMXON_CR4_ALWAYSON;
		break;
	case MSR_IA32_VMX_CR4_FIXED1:
		*pdata = -1ULL;
		break;
	case MSR_IA32_VMX_VMCS_ENUM:
		*pdata = 0x2e; /* highest index: VMX_PREEMPTION_TIMER_VALUE */
		break;
	case MSR_IA32_VMX_PROCBASED_CTLS2:
		*pdata = vmx_control_msr(
			vmx->nested.nested_vmx_secondary_ctls_low,
			vmx->nested.nested_vmx_secondary_ctls_high);
		break;
	case MSR_IA32_VMX_EPT_VPID_CAP:
		/* Currently, no nested vpid support */
		*pdata = vmx->nested.nested_vmx_ept_caps |
			((u64)vmx->nested.nested_vmx_vpid_caps << 32);
		break;
	default:
		return 1;
	}

	return 0;
}
