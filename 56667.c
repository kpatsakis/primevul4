static int vmx_set_msr(struct kvm_vcpu *vcpu, struct msr_data *msr_info)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	struct shared_msr_entry *msr;
	int ret = 0;
	u32 msr_index = msr_info->index;
	u64 data = msr_info->data;

	switch (msr_index) {
	case MSR_EFER:
		ret = kvm_set_msr_common(vcpu, msr_info);
		break;
#ifdef CONFIG_X86_64
	case MSR_FS_BASE:
		vmx_segment_cache_clear(vmx);
		vmcs_writel(GUEST_FS_BASE, data);
		break;
	case MSR_GS_BASE:
		vmx_segment_cache_clear(vmx);
		vmcs_writel(GUEST_GS_BASE, data);
		break;
	case MSR_KERNEL_GS_BASE:
		vmx_load_host_state(vmx);
		vmx->msr_guest_kernel_gs_base = data;
		break;
#endif
	case MSR_IA32_SYSENTER_CS:
		vmcs_write32(GUEST_SYSENTER_CS, data);
		break;
	case MSR_IA32_SYSENTER_EIP:
		vmcs_writel(GUEST_SYSENTER_EIP, data);
		break;
	case MSR_IA32_SYSENTER_ESP:
		vmcs_writel(GUEST_SYSENTER_ESP, data);
		break;
	case MSR_IA32_BNDCFGS:
		if (!kvm_mpx_supported() ||
		    (!msr_info->host_initiated &&
		     !guest_cpuid_has(vcpu, X86_FEATURE_MPX)))
			return 1;
		if (is_noncanonical_address(data & PAGE_MASK, vcpu) ||
		    (data & MSR_IA32_BNDCFGS_RSVD))
			return 1;
		vmcs_write64(GUEST_BNDCFGS, data);
		break;
	case MSR_IA32_TSC:
		kvm_write_tsc(vcpu, msr_info);
		break;
	case MSR_IA32_CR_PAT:
		if (vmcs_config.vmentry_ctrl & VM_ENTRY_LOAD_IA32_PAT) {
			if (!kvm_mtrr_valid(vcpu, MSR_IA32_CR_PAT, data))
				return 1;
			vmcs_write64(GUEST_IA32_PAT, data);
			vcpu->arch.pat = data;
			break;
		}
		ret = kvm_set_msr_common(vcpu, msr_info);
		break;
	case MSR_IA32_TSC_ADJUST:
		ret = kvm_set_msr_common(vcpu, msr_info);
		break;
	case MSR_IA32_MCG_EXT_CTL:
		if ((!msr_info->host_initiated &&
		     !(to_vmx(vcpu)->msr_ia32_feature_control &
		       FEATURE_CONTROL_LMCE)) ||
		    (data & ~MCG_EXT_CTL_LMCE_EN))
			return 1;
		vcpu->arch.mcg_ext_ctl = data;
		break;
	case MSR_IA32_FEATURE_CONTROL:
		if (!vmx_feature_control_msr_valid(vcpu, data) ||
		    (to_vmx(vcpu)->msr_ia32_feature_control &
		     FEATURE_CONTROL_LOCKED && !msr_info->host_initiated))
			return 1;
		vmx->msr_ia32_feature_control = data;
		if (msr_info->host_initiated && data == 0)
			vmx_leave_nested(vcpu);
		break;
	case MSR_IA32_VMX_BASIC ... MSR_IA32_VMX_VMFUNC:
		if (!msr_info->host_initiated)
			return 1; /* they are read-only */
		if (!nested_vmx_allowed(vcpu))
			return 1;
		return vmx_set_vmx_msr(vcpu, msr_index, data);
	case MSR_IA32_XSS:
		if (!vmx_xsaves_supported())
			return 1;
		/*
		 * The only supported bit as of Skylake is bit 8, but
		 * it is not supported on KVM.
		 */
		if (data != 0)
			return 1;
		vcpu->arch.ia32_xss = data;
		if (vcpu->arch.ia32_xss != host_xss)
			add_atomic_switch_msr(vmx, MSR_IA32_XSS,
				vcpu->arch.ia32_xss, host_xss);
		else
			clear_atomic_switch_msr(vmx, MSR_IA32_XSS);
		break;
	case MSR_TSC_AUX:
		if (!msr_info->host_initiated &&
		    !guest_cpuid_has(vcpu, X86_FEATURE_RDTSCP))
			return 1;
		/* Check reserved bit, higher 32 bits should be zero */
		if ((data >> 32) != 0)
			return 1;
		/* Otherwise falls through */
	default:
		msr = find_msr_entry(vmx, msr_index);
		if (msr) {
			u64 old_msr_data = msr->data;
			msr->data = data;
			if (msr - vmx->guest_msrs < vmx->save_nmsrs) {
				preempt_disable();
				ret = kvm_set_shared_msr(msr->index, msr->data,
							 msr->mask);
				preempt_enable();
				if (ret)
					msr->data = old_msr_data;
			}
			break;
		}
		ret = kvm_set_msr_common(vcpu, msr_info);
	}

	return ret;
}
