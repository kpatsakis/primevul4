static __init int hardware_setup(void)
{
	if (setup_vmcs_config(&vmcs_config) < 0)
		return -EIO;

	if (boot_cpu_has(X86_FEATURE_NX))
		kvm_enable_efer_bits(EFER_NX);

	if (!cpu_has_vmx_vpid())
		enable_vpid = 0;
	if (!cpu_has_vmx_shadow_vmcs())
		enable_shadow_vmcs = 0;

	if (!cpu_has_vmx_ept() ||
	    !cpu_has_vmx_ept_4levels()) {
		enable_ept = 0;
		enable_unrestricted_guest = 0;
		enable_ept_ad_bits = 0;
	}

	if (!cpu_has_vmx_ept_ad_bits())
		enable_ept_ad_bits = 0;

	if (!cpu_has_vmx_unrestricted_guest())
		enable_unrestricted_guest = 0;

	if (!cpu_has_vmx_flexpriority())
		flexpriority_enabled = 0;

	if (!cpu_has_vmx_tpr_shadow())
		kvm_x86_ops->update_cr8_intercept = NULL;

	if (enable_ept && !cpu_has_vmx_ept_2m_page())
		kvm_disable_largepages();

	if (!cpu_has_vmx_ple())
		ple_gap = 0;

	if (!cpu_has_vmx_apicv())
		enable_apicv = 0;

	if (enable_apicv)
		kvm_x86_ops->update_cr8_intercept = NULL;
	else {
		kvm_x86_ops->hwapic_irr_update = NULL;
		kvm_x86_ops->deliver_posted_interrupt = NULL;
		kvm_x86_ops->sync_pir_to_irr = vmx_sync_pir_to_irr_dummy;
	}

	if (nested)
		nested_vmx_setup_ctls_msrs();

	return alloc_kvm_area();
}
