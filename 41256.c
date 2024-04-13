void kvm_free_physmem(struct kvm *kvm)
{
	int i;
	struct kvm_memslots *slots = kvm->memslots;

	for (i = 0; i < slots->nmemslots; ++i)
		kvm_free_physmem_slot(&slots->memslots[i], NULL);

	kfree(kvm->memslots);
}
