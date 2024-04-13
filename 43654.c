static bool __rmap_write_protect(struct kvm *kvm, unsigned long *rmapp,
				 bool pt_protect)
{
	u64 *sptep;
	struct rmap_iterator iter;
	bool flush = false;

	for (sptep = rmap_get_first(*rmapp, &iter); sptep;) {
		BUG_ON(!(*sptep & PT_PRESENT_MASK));
		if (spte_write_protect(kvm, sptep, &flush, pt_protect)) {
			sptep = rmap_get_first(*rmapp, &iter);
			continue;
		}

		sptep = rmap_get_next(&iter);
	}

	return flush;
}
