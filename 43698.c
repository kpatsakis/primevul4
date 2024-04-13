static bool is_obsolete_sp(struct kvm *kvm, struct kvm_mmu_page *sp)
{
	return unlikely(sp->mmu_valid_gen != kvm->arch.mmu_valid_gen);
}
