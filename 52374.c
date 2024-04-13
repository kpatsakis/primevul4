static void mtrr_lookup_init(struct mtrr_iter *iter,
			     struct kvm_mtrr *mtrr_state, u64 start, u64 end)
{
	iter->mtrr_state = mtrr_state;
	iter->start = start;
	iter->end = end;
	iter->mtrr_disabled = false;
	iter->partial_map = false;
	iter->fixed = false;
	iter->range = NULL;

	mtrr_lookup_start(iter);
}
