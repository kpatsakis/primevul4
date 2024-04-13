static int kvm_handle_hva_range(struct kvm *kvm,
				unsigned long start,
				unsigned long end,
				unsigned long data,
				int (*handler)(struct kvm *kvm,
					       unsigned long *rmapp,
					       struct kvm_memory_slot *slot,
					       unsigned long data))
{
	int j;
	int ret = 0;
	struct kvm_memslots *slots;
	struct kvm_memory_slot *memslot;

	slots = kvm_memslots(kvm);

	kvm_for_each_memslot(memslot, slots) {
		unsigned long hva_start, hva_end;
		gfn_t gfn_start, gfn_end;

		hva_start = max(start, memslot->userspace_addr);
		hva_end = min(end, memslot->userspace_addr +
					(memslot->npages << PAGE_SHIFT));
		if (hva_start >= hva_end)
			continue;
		/*
		 * {gfn(page) | page intersects with [hva_start, hva_end)} =
		 * {gfn_start, gfn_start+1, ..., gfn_end-1}.
		 */
		gfn_start = hva_to_gfn_memslot(hva_start, memslot);
		gfn_end = hva_to_gfn_memslot(hva_end + PAGE_SIZE - 1, memslot);

		for (j = PT_PAGE_TABLE_LEVEL;
		     j < PT_PAGE_TABLE_LEVEL + KVM_NR_PAGE_SIZES; ++j) {
			unsigned long idx, idx_end;
			unsigned long *rmapp;

			/*
			 * {idx(page_j) | page_j intersects with
			 *  [hva_start, hva_end)} = {idx, idx+1, ..., idx_end}.
			 */
			idx = gfn_to_index(gfn_start, memslot->base_gfn, j);
			idx_end = gfn_to_index(gfn_end - 1, memslot->base_gfn, j);

			rmapp = __gfn_to_rmap(gfn_start, j, memslot);

			for (; idx <= idx_end; ++idx)
				ret |= handler(kvm, rmapp++, memslot, data);
		}
	}

	return ret;
}
