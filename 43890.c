static int vmx_get_vmx_msr(struct kvm_vcpu *vcpu, u32 msr_index, u64 *pdata)
{
	if (!nested_vmx_allowed(vcpu) && msr_index >= MSR_IA32_VMX_BASIC &&
		     msr_index <= MSR_IA32_VMX_TRUE_ENTRY_CTLS) {
		/*
		 * According to the spec, processors which do not support VMX
		 * should throw a #GP(0) when VMX capability MSRs are read.
		 */
		kvm_queue_exception_e(vcpu, GP_VECTOR, 0);
		return 1;
	}

	switch (msr_index) {
	case MSR_IA32_FEATURE_CONTROL:
		if (nested_vmx_allowed(vcpu)) {
			*pdata = to_vmx(vcpu)->nested.msr_ia32_feature_control;
			break;
		}
		return 0;
	case MSR_IA32_VMX_BASIC:
		/*
		 * This MSR reports some information about VMX support. We
		 * should return information about the VMX we emulate for the
		 * guest, and the VMCS structure we give it - not about the
		 * VMX support of the underlying hardware.
		 */
		*pdata = VMCS12_REVISION |
			   ((u64)VMCS12_SIZE << VMX_BASIC_VMCS_SIZE_SHIFT) |
			   (VMX_BASIC_MEM_TYPE_WB << VMX_BASIC_MEM_TYPE_SHIFT);
		break;
	case MSR_IA32_VMX_TRUE_PINBASED_CTLS:
	case MSR_IA32_VMX_PINBASED_CTLS:
		*pdata = vmx_control_msr(nested_vmx_pinbased_ctls_low,
					nested_vmx_pinbased_ctls_high);
		break;
	case MSR_IA32_VMX_TRUE_PROCBASED_CTLS:
	case MSR_IA32_VMX_PROCBASED_CTLS:
		*pdata = vmx_control_msr(nested_vmx_procbased_ctls_low,
					nested_vmx_procbased_ctls_high);
		break;
	case MSR_IA32_VMX_TRUE_EXIT_CTLS:
	case MSR_IA32_VMX_EXIT_CTLS:
		*pdata = vmx_control_msr(nested_vmx_exit_ctls_low,
					nested_vmx_exit_ctls_high);
		break;
	case MSR_IA32_VMX_TRUE_ENTRY_CTLS:
	case MSR_IA32_VMX_ENTRY_CTLS:
		*pdata = vmx_control_msr(nested_vmx_entry_ctls_low,
					nested_vmx_entry_ctls_high);
		break;
	case MSR_IA32_VMX_MISC:
		*pdata = vmx_control_msr(nested_vmx_misc_low,
					 nested_vmx_misc_high);
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
		*pdata = 0x1f;
		break;
	case MSR_IA32_VMX_PROCBASED_CTLS2:
		*pdata = vmx_control_msr(nested_vmx_secondary_ctls_low,
					nested_vmx_secondary_ctls_high);
		break;
	case MSR_IA32_VMX_EPT_VPID_CAP:
		/* Currently, no nested ept or nested vpid */
		*pdata = 0;
		break;
	default:
		return 0;
	}

	return 1;
}
