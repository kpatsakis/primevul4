static void vmx_flush_tlb_ept_only(struct kvm_vcpu *vcpu)
{
	if (enable_ept)
		vmx_flush_tlb(vcpu);
}
