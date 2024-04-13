static void drop_large_spte(struct kvm_vcpu *vcpu, u64 *sptep)
{
	if (__drop_large_spte(vcpu->kvm, sptep))
		kvm_flush_remote_tlbs(vcpu->kvm);
}
