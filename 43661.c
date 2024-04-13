static bool check_mmio_spte(struct kvm *kvm, u64 spte)
{
	unsigned int kvm_gen, spte_gen;

	kvm_gen = kvm_current_mmio_generation(kvm);
	spte_gen = get_mmio_spte_generation(spte);

	trace_check_mmio_spte(spte, kvm_gen, spte_gen);
	return likely(kvm_gen == spte_gen);
}
