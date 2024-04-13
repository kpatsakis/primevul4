static struct kvm_memory_slot *__gfn_to_memslot(struct kvm_memslots *slots,
						gfn_t gfn)
{
	int i;

	for (i = 0; i < slots->nmemslots; ++i) {
		struct kvm_memory_slot *memslot = &slots->memslots[i];

		if (gfn >= memslot->base_gfn
		    && gfn < memslot->base_gfn + memslot->npages)
			return memslot;
	}
	return NULL;
}
