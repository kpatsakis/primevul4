static void vmx_hwapic_irr_update(struct kvm_vcpu *vcpu, int max_irr)
{
	if (max_irr == -1)
		return;

	vmx_set_rvi(max_irr);
}
