static void mark_map_reg(struct bpf_reg_state *regs, u32 regno, u32 id,
			 enum bpf_reg_type type)
{
	struct bpf_reg_state *reg = &regs[regno];

	if (reg->type == PTR_TO_MAP_VALUE_OR_NULL && reg->id == id) {
		if (type == UNKNOWN_VALUE) {
			__mark_reg_unknown_value(regs, regno);
		} else if (reg->map_ptr->inner_map_meta) {
			reg->type = CONST_PTR_TO_MAP;
			reg->map_ptr = reg->map_ptr->inner_map_meta;
		} else {
			reg->type = type;
		}
		/* We don't need id from this point onwards anymore, thus we
		 * should better reset it, so that state pruning has chances
		 * to take effect.
		 */
		reg->id = 0;
	}
}
