static void mtrr_lookup_var_start(struct mtrr_iter *iter)
{
	struct kvm_mtrr *mtrr_state = iter->mtrr_state;

	iter->fixed = false;
	iter->start_max = iter->start;
	iter->range = list_prepare_entry(iter->range, &mtrr_state->head, node);

	__mtrr_lookup_var_next(iter);
}
