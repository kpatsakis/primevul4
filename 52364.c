u8 kvm_mtrr_get_guest_memory_type(struct kvm_vcpu *vcpu, gfn_t gfn)
{
	struct kvm_mtrr *mtrr_state = &vcpu->arch.mtrr_state;
	struct mtrr_iter iter;
	u64 start, end;
	int type = -1;
	const int wt_wb_mask = (1 << MTRR_TYPE_WRBACK)
			       | (1 << MTRR_TYPE_WRTHROUGH);

	start = gfn_to_gpa(gfn);
	end = start + PAGE_SIZE;

	mtrr_for_each_mem_type(&iter, mtrr_state, start, end) {
		int curr_type = iter.mem_type;

		/*
		 * Please refer to Intel SDM Volume 3: 11.11.4.1 MTRR
		 * Precedences.
		 */

		if (type == -1) {
			type = curr_type;
			continue;
		}

		/*
		 * If two or more variable memory ranges match and the
		 * memory types are identical, then that memory type is
		 * used.
		 */
		if (type == curr_type)
			continue;

		/*
		 * If two or more variable memory ranges match and one of
		 * the memory types is UC, the UC memory type used.
		 */
		if (curr_type == MTRR_TYPE_UNCACHABLE)
			return MTRR_TYPE_UNCACHABLE;

		/*
		 * If two or more variable memory ranges match and the
		 * memory types are WT and WB, the WT memory type is used.
		 */
		if (((1 << type) & wt_wb_mask) &&
		      ((1 << curr_type) & wt_wb_mask)) {
			type = MTRR_TYPE_WRTHROUGH;
			continue;
		}

		/*
		 * For overlaps not defined by the above rules, processor
		 * behavior is undefined.
		 */

		/* We use WB for this undefined behavior. :( */
		return MTRR_TYPE_WRBACK;
	}

	if (iter.mtrr_disabled)
		return mtrr_disabled_type(vcpu);

	/* not contained in any MTRRs. */
	if (type == -1)
		return mtrr_default_type(mtrr_state);

	/*
	 * We just check one page, partially covered by MTRRs is
	 * impossible.
	 */
	WARN_ON(iter.partial_map);

	return type;
}
