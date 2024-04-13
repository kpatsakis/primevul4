void kvm_mmu_invlpg(struct kvm_vcpu *vcpu, gva_t gva)
{
	vcpu->arch.mmu.invlpg(vcpu, gva);
	kvm_mmu_flush_tlb(vcpu);
	++vcpu->stat.invlpg;
}
