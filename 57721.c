static void mark_map_regs(struct bpf_verifier_state *state, u32 regno,
			  enum bpf_reg_type type)
{
	struct bpf_reg_state *regs = state->regs;
	u32 id = regs[regno].id;
	int i;

	for (i = 0; i < MAX_BPF_REG; i++)
		mark_map_reg(regs, i, id, type);

	for (i = 0; i < MAX_BPF_STACK; i += BPF_REG_SIZE) {
		if (state->stack_slot_type[i] != STACK_SPILL)
			continue;
		mark_map_reg(state->spilled_regs, i / BPF_REG_SIZE, id, type);
	}
}
