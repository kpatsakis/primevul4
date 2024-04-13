static void __find_good_pkt_pointers(struct bpf_func_state *state,
				     struct bpf_reg_state *dst_reg,
				     enum bpf_reg_type type, int new_range)
{
	struct bpf_reg_state *reg;
	int i;

	for (i = 0; i < MAX_BPF_REG; i++) {
		reg = &state->regs[i];
		if (reg->type == type && reg->id == dst_reg->id)
			/* keep the maximum range already checked */
			reg->range = max(reg->range, new_range);
	}

	bpf_for_each_spilled_reg(i, state, reg) {
		if (!reg)
			continue;
		if (reg->type == type && reg->id == dst_reg->id)
			reg->range = max(reg->range, new_range);
	}
}