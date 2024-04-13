static void __scalar64_min_max_lsh(struct bpf_reg_state *dst_reg,
				   u64 umin_val, u64 umax_val)
{
	/* Special case <<32 because it is a common compiler pattern to sign
	 * extend subreg by doing <<32 s>>32. In this case if 32bit bounds are
	 * positive we know this shift will also be positive so we can track
	 * bounds correctly. Otherwise we lose all sign bit information except
	 * what we can pick up from var_off. Perhaps we can generalize this
	 * later to shifts of any length.
	 */
	if (umin_val == 32 && umax_val == 32 && dst_reg->s32_max_value >= 0)
		dst_reg->smax_value = (s64)dst_reg->s32_max_value << 32;
	else
		dst_reg->smax_value = S64_MAX;

	if (umin_val == 32 && umax_val == 32 && dst_reg->s32_min_value >= 0)
		dst_reg->smin_value = (s64)dst_reg->s32_min_value << 32;
	else
		dst_reg->smin_value = S64_MIN;

	/* If we might shift our top bit out, then we know nothing */
	if (dst_reg->umax_value > 1ULL << (63 - umax_val)) {
		dst_reg->umin_value = 0;
		dst_reg->umax_value = U64_MAX;
	} else {
		dst_reg->umin_value <<= umin_val;
		dst_reg->umax_value <<= umax_val;
	}
}