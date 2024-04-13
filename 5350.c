static void find_equal_scalars(struct bpf_verifier_state *vstate,
			       struct bpf_reg_state *known_reg)
{
	struct bpf_func_state *state;
	struct bpf_reg_state *reg;
	int i, j;

	for (i = 0; i <= vstate->curframe; i++) {
		state = vstate->frame[i];
		for (j = 0; j < MAX_BPF_REG; j++) {
			reg = &state->regs[j];
			if (reg->type == SCALAR_VALUE && reg->id == known_reg->id)
				*reg = *known_reg;
		}

		bpf_for_each_spilled_reg(j, state, reg) {
			if (!reg)
				continue;
			if (reg->type == SCALAR_VALUE && reg->id == known_reg->id)
				*reg = *known_reg;
		}
	}
}