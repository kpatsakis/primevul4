static void vmx_cpuid_update(struct kvm_vcpu *vcpu)
{
	struct kvm_cpuid_entry2 *best;
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	u32 secondary_exec_ctl = vmx_secondary_exec_control(vmx);

	if (vmx_rdtscp_supported()) {
		bool rdtscp_enabled = guest_cpuid_has_rdtscp(vcpu);
		if (!rdtscp_enabled)
			secondary_exec_ctl &= ~SECONDARY_EXEC_RDTSCP;

		if (nested) {
			if (rdtscp_enabled)
				vmx->nested.nested_vmx_secondary_ctls_high |=
					SECONDARY_EXEC_RDTSCP;
			else
				vmx->nested.nested_vmx_secondary_ctls_high &=
					~SECONDARY_EXEC_RDTSCP;
		}
	}

	/* Exposing INVPCID only when PCID is exposed */
	best = kvm_find_cpuid_entry(vcpu, 0x7, 0);
	if (vmx_invpcid_supported() &&
	    (!best || !(best->ebx & bit(X86_FEATURE_INVPCID)) ||
	    !guest_cpuid_has_pcid(vcpu))) {
		secondary_exec_ctl &= ~SECONDARY_EXEC_ENABLE_INVPCID;

		if (best)
			best->ebx &= ~bit(X86_FEATURE_INVPCID);
	}

	vmcs_set_secondary_exec_control(secondary_exec_ctl);

	if (static_cpu_has(X86_FEATURE_PCOMMIT) && nested) {
		if (guest_cpuid_has_pcommit(vcpu))
			vmx->nested.nested_vmx_secondary_ctls_high |=
				SECONDARY_EXEC_PCOMMIT;
		else
			vmx->nested.nested_vmx_secondary_ctls_high &=
				~SECONDARY_EXEC_PCOMMIT;
	}
}
