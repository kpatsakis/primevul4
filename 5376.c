static void __reg_assign_32_into_64(struct bpf_reg_state *reg)
{
	reg->umin_value = reg->u32_min_value;
	reg->umax_value = reg->u32_max_value;

	/* Attempt to pull 32-bit signed bounds into 64-bit bounds but must
	 * be positive otherwise set to worse case bounds and refine later
	 * from tnum.
	 */
	if (__reg32_bound_s64(reg->s32_min_value) &&
	    __reg32_bound_s64(reg->s32_max_value)) {
		reg->smin_value = reg->s32_min_value;
		reg->smax_value = reg->s32_max_value;
	} else {
		reg->smin_value = 0;
		reg->smax_value = U32_MAX;
	}
}