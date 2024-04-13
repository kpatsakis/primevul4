static int nested_pf_handled(struct kvm_vcpu *vcpu)
{
	struct vmcs12 *vmcs12 = get_vmcs12(vcpu);

	/* TODO: also check PFEC_MATCH/MASK, not just EB.PF. */
	if (!(vmcs12->exception_bitmap & (1u << PF_VECTOR)))
		return 0;

	nested_vmx_vmexit(vcpu);
	return 1;
}
