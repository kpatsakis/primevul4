static void scalar32_min_max_mul(struct bpf_reg_state *dst_reg,
				 struct bpf_reg_state *src_reg)
{
	s32 smin_val = src_reg->s32_min_value;
	u32 umin_val = src_reg->u32_min_value;
	u32 umax_val = src_reg->u32_max_value;

	if (smin_val < 0 || dst_reg->s32_min_value < 0) {
		/* Ain't nobody got time to multiply that sign */
		__mark_reg32_unbounded(dst_reg);
		return;
	}
	/* Both values are positive, so we can work with unsigned and
	 * copy the result to signed (unless it exceeds S32_MAX).
	 */
	if (umax_val > U16_MAX || dst_reg->u32_max_value > U16_MAX) {
		/* Potential overflow, we know nothing */
		__mark_reg32_unbounded(dst_reg);
		return;
	}
	dst_reg->u32_min_value *= umin_val;
	dst_reg->u32_max_value *= umax_val;
	if (dst_reg->u32_max_value > S32_MAX) {
		/* Overflow possible, we know nothing */
		dst_reg->s32_min_value = S32_MIN;
		dst_reg->s32_max_value = S32_MAX;
	} else {
		dst_reg->s32_min_value = dst_reg->u32_min_value;
		dst_reg->s32_max_value = dst_reg->u32_max_value;
	}
}