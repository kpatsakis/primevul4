static int nested_vmx_check_apicv_controls(struct kvm_vcpu *vcpu,
					   struct vmcs12 *vmcs12)
{
	if (!nested_cpu_has_virt_x2apic_mode(vmcs12) &&
	    !nested_cpu_has_apic_reg_virt(vmcs12) &&
	    !nested_cpu_has_vid(vmcs12) &&
	    !nested_cpu_has_posted_intr(vmcs12))
		return 0;

	/*
	 * If virtualize x2apic mode is enabled,
	 * virtualize apic access must be disabled.
	 */
	if (nested_cpu_has_virt_x2apic_mode(vmcs12) &&
	    nested_cpu_has2(vmcs12, SECONDARY_EXEC_VIRTUALIZE_APIC_ACCESSES))
		return -EINVAL;

	/*
	 * If virtual interrupt delivery is enabled,
	 * we must exit on external interrupts.
	 */
	if (nested_cpu_has_vid(vmcs12) &&
	   !nested_exit_on_intr(vcpu))
		return -EINVAL;

	/*
	 * bits 15:8 should be zero in posted_intr_nv,
	 * the descriptor address has been already checked
	 * in nested_get_vmcs12_pages.
	 */
	if (nested_cpu_has_posted_intr(vmcs12) &&
	   (!nested_cpu_has_vid(vmcs12) ||
	    !nested_exit_intr_ack_set(vcpu) ||
	    vmcs12->posted_intr_nv & 0xff00))
		return -EINVAL;

	/* tpr shadow is needed by all apicv features. */
	if (!nested_cpu_has(vmcs12, CPU_BASED_TPR_SHADOW))
		return -EINVAL;

	return 0;
}
