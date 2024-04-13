static bool __is_scalar_unbounded(struct bpf_reg_state *reg)
{
	return tnum_is_unknown(reg->var_off) &&
	       reg->smin_value == S64_MIN && reg->smax_value == S64_MAX &&
	       reg->umin_value == 0 && reg->umax_value == U64_MAX &&
	       reg->s32_min_value == S32_MIN && reg->s32_max_value == S32_MAX &&
	       reg->u32_min_value == 0 && reg->u32_max_value == U32_MAX;
}