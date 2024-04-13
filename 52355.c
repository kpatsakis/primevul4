static void __mtrr_lookup_var_next(struct mtrr_iter *iter)
{
	struct kvm_mtrr *mtrr_state = iter->mtrr_state;

	list_for_each_entry_continue(iter->range, &mtrr_state->head, node)
		if (match_var_range(iter, iter->range))
			return;

	iter->range = NULL;
	iter->partial_map |= iter->start_max < iter->end;
}
