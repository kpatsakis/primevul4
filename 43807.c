static void paging_free(struct kvm_vcpu *vcpu)
{
	nonpaging_free(vcpu);
}
