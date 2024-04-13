bool kvm_mtrr_check_gfn_range_consistency(struct kvm_vcpu *vcpu, gfn_t gfn,
					  int page_num)
{
	struct kvm_mtrr *mtrr_state = &vcpu->arch.mtrr_state;
	struct mtrr_iter iter;
	u64 start, end;
	int type = -1;

	start = gfn_to_gpa(gfn);
	end = gfn_to_gpa(gfn + page_num);
	mtrr_for_each_mem_type(&iter, mtrr_state, start, end) {
		if (type == -1) {
			type = iter.mem_type;
			continue;
		}

		if (type != iter.mem_type)
			return false;
	}

	if (iter.mtrr_disabled)
		return true;

	if (!iter.partial_map)
		return true;

	if (type == -1)
		return true;

	return type == mtrr_default_type(mtrr_state);
}
