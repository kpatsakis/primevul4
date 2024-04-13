static void vmx_compute_secondary_exec_control(struct vcpu_vmx *vmx)
{
	struct kvm_vcpu *vcpu = &vmx->vcpu;

	u32 exec_control = vmcs_config.cpu_based_2nd_exec_ctrl;
	if (!cpu_need_virtualize_apic_accesses(vcpu))
		exec_control &= ~SECONDARY_EXEC_VIRTUALIZE_APIC_ACCESSES;
	if (vmx->vpid == 0)
		exec_control &= ~SECONDARY_EXEC_ENABLE_VPID;
	if (!enable_ept) {
		exec_control &= ~SECONDARY_EXEC_ENABLE_EPT;
		enable_unrestricted_guest = 0;
		/* Enable INVPCID for non-ept guests may cause performance regression. */
		exec_control &= ~SECONDARY_EXEC_ENABLE_INVPCID;
	}
	if (!enable_unrestricted_guest)
		exec_control &= ~SECONDARY_EXEC_UNRESTRICTED_GUEST;
	if (!ple_gap)
		exec_control &= ~SECONDARY_EXEC_PAUSE_LOOP_EXITING;
	if (!kvm_vcpu_apicv_active(vcpu))
		exec_control &= ~(SECONDARY_EXEC_APIC_REGISTER_VIRT |
				  SECONDARY_EXEC_VIRTUAL_INTR_DELIVERY);
	exec_control &= ~SECONDARY_EXEC_VIRTUALIZE_X2APIC_MODE;
	/* SECONDARY_EXEC_SHADOW_VMCS is enabled when L1 executes VMPTRLD
	   (handle_vmptrld).
	   We can NOT enable shadow_vmcs here because we don't have yet
	   a current VMCS12
	*/
	exec_control &= ~SECONDARY_EXEC_SHADOW_VMCS;

	if (!enable_pml)
		exec_control &= ~SECONDARY_EXEC_ENABLE_PML;

	if (vmx_xsaves_supported()) {
		/* Exposing XSAVES only when XSAVE is exposed */
		bool xsaves_enabled =
			guest_cpuid_has(vcpu, X86_FEATURE_XSAVE) &&
			guest_cpuid_has(vcpu, X86_FEATURE_XSAVES);

		if (!xsaves_enabled)
			exec_control &= ~SECONDARY_EXEC_XSAVES;

		if (nested) {
			if (xsaves_enabled)
				vmx->nested.nested_vmx_secondary_ctls_high |=
					SECONDARY_EXEC_XSAVES;
			else
				vmx->nested.nested_vmx_secondary_ctls_high &=
					~SECONDARY_EXEC_XSAVES;
		}
	}

	if (vmx_rdtscp_supported()) {
		bool rdtscp_enabled = guest_cpuid_has(vcpu, X86_FEATURE_RDTSCP);
		if (!rdtscp_enabled)
			exec_control &= ~SECONDARY_EXEC_RDTSCP;

		if (nested) {
			if (rdtscp_enabled)
				vmx->nested.nested_vmx_secondary_ctls_high |=
					SECONDARY_EXEC_RDTSCP;
			else
				vmx->nested.nested_vmx_secondary_ctls_high &=
					~SECONDARY_EXEC_RDTSCP;
		}
	}

	if (vmx_invpcid_supported()) {
		/* Exposing INVPCID only when PCID is exposed */
		bool invpcid_enabled =
			guest_cpuid_has(vcpu, X86_FEATURE_INVPCID) &&
			guest_cpuid_has(vcpu, X86_FEATURE_PCID);

		if (!invpcid_enabled) {
			exec_control &= ~SECONDARY_EXEC_ENABLE_INVPCID;
			guest_cpuid_clear(vcpu, X86_FEATURE_INVPCID);
		}

		if (nested) {
			if (invpcid_enabled)
				vmx->nested.nested_vmx_secondary_ctls_high |=
					SECONDARY_EXEC_ENABLE_INVPCID;
			else
				vmx->nested.nested_vmx_secondary_ctls_high &=
					~SECONDARY_EXEC_ENABLE_INVPCID;
		}
	}

	if (vmx_rdrand_supported()) {
		bool rdrand_enabled = guest_cpuid_has(vcpu, X86_FEATURE_RDRAND);
		if (rdrand_enabled)
			exec_control &= ~SECONDARY_EXEC_RDRAND;

		if (nested) {
			if (rdrand_enabled)
				vmx->nested.nested_vmx_secondary_ctls_high |=
					SECONDARY_EXEC_RDRAND;
			else
				vmx->nested.nested_vmx_secondary_ctls_high &=
					~SECONDARY_EXEC_RDRAND;
		}
	}

	if (vmx_rdseed_supported()) {
		bool rdseed_enabled = guest_cpuid_has(vcpu, X86_FEATURE_RDSEED);
		if (rdseed_enabled)
			exec_control &= ~SECONDARY_EXEC_RDSEED;

		if (nested) {
			if (rdseed_enabled)
				vmx->nested.nested_vmx_secondary_ctls_high |=
					SECONDARY_EXEC_RDSEED;
			else
				vmx->nested.nested_vmx_secondary_ctls_high &=
					~SECONDARY_EXEC_RDSEED;
		}
	}

	vmx->secondary_exec_control = exec_control;
}
