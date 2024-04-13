void kvm_arch_free_memslot(struct kvm *kvm, struct kvm_memory_slot *free,
			   struct kvm_memory_slot *dont)
{
	int i;

	for (i = 0; i < KVM_NR_PAGE_SIZES; ++i) {
		if (!dont || free->arch.rmap[i] != dont->arch.rmap[i]) {
			kvfree(free->arch.rmap[i]);
			free->arch.rmap[i] = NULL;
		}
		if (i == 0)
			continue;

		if (!dont || free->arch.lpage_info[i - 1] !=
			     dont->arch.lpage_info[i - 1]) {
			kvfree(free->arch.lpage_info[i - 1]);
			free->arch.lpage_info[i - 1] = NULL;
		}
	}
}
