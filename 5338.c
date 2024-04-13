static bool register_is_bounded(struct bpf_reg_state *reg)
{
	return reg->type == SCALAR_VALUE && !__is_scalar_unbounded(reg);
}