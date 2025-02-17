void kvm_mmu_sync_roots(struct kvm_vcpu *vcpu)
{
	spin_lock(&vcpu->kvm->mmu_lock);
 	mmu_sync_roots(vcpu);
 	spin_unlock(&vcpu->kvm->mmu_lock);
 }
