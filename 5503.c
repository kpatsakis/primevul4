static void scalar_min_max_sub(struct bpf_reg_state *dst_reg,
			       struct bpf_reg_state *src_reg)
{
	s64 smin_val = src_reg->smin_value;
	s64 smax_val = src_reg->smax_value;
	u64 umin_val = src_reg->umin_value;
	u64 umax_val = src_reg->umax_value;

	if (signed_sub_overflows(dst_reg->smin_value, smax_val) ||
	    signed_sub_overflows(dst_reg->smax_value, smin_val)) {
		/* Overflow possible, we know nothing */
		dst_reg->smin_value = S64_MIN;
		dst_reg->smax_value = S64_MAX;
	} else {
		dst_reg->smin_value -= smax_val;
		dst_reg->smax_value -= smin_val;
	}
	if (dst_reg->umin_value < umax_val) {
		/* Overflow possible, we know nothing */
		dst_reg->umin_value = 0;
		dst_reg->umax_value = U64_MAX;
	} else {
		/* Cannot overflow (as long as bounds are consistent) */
		dst_reg->umin_value -= umax_val;
		dst_reg->umax_value -= umin_val;
	}
}