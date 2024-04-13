static void scalar32_min_max_add(struct bpf_reg_state *dst_reg,
				 struct bpf_reg_state *src_reg)
{
	s32 smin_val = src_reg->s32_min_value;
	s32 smax_val = src_reg->s32_max_value;
	u32 umin_val = src_reg->u32_min_value;
	u32 umax_val = src_reg->u32_max_value;

	if (signed_add32_overflows(dst_reg->s32_min_value, smin_val) ||
	    signed_add32_overflows(dst_reg->s32_max_value, smax_val)) {
		dst_reg->s32_min_value = S32_MIN;
		dst_reg->s32_max_value = S32_MAX;
	} else {
		dst_reg->s32_min_value += smin_val;
		dst_reg->s32_max_value += smax_val;
	}
	if (dst_reg->u32_min_value + umin_val < umin_val ||
	    dst_reg->u32_max_value + umax_val < umax_val) {
		dst_reg->u32_min_value = 0;
		dst_reg->u32_max_value = U32_MAX;
	} else {
		dst_reg->u32_min_value += umin_val;
		dst_reg->u32_max_value += umax_val;
	}
}