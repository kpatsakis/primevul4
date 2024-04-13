static void vmx_cpuid_update(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	if (cpu_has_secondary_exec_ctrls()) {
		vmx_compute_secondary_exec_control(vmx);
		vmcs_set_secondary_exec_control(vmx->secondary_exec_control);
	}

	if (nested_vmx_allowed(vcpu))
		to_vmx(vcpu)->msr_ia32_feature_control_valid_bits |=
			FEATURE_CONTROL_VMXON_ENABLED_OUTSIDE_SMX;
	else
		to_vmx(vcpu)->msr_ia32_feature_control_valid_bits &=
			~FEATURE_CONTROL_VMXON_ENABLED_OUTSIDE_SMX;

	if (nested_vmx_allowed(vcpu))
		nested_vmx_cr_fixed1_bits_update(vcpu);
}
