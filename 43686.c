static int has_wrprotected_page(struct kvm *kvm,
				gfn_t gfn,
				int level)
{
	struct kvm_memory_slot *slot;
	struct kvm_lpage_info *linfo;

	slot = gfn_to_memslot(kvm, gfn);
	if (slot) {
		linfo = lpage_info_slot(gfn, slot, level);
		return linfo->write_count;
	}

	return 1;
}
