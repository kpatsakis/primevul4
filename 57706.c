static void clear_all_pkt_pointers(struct bpf_verifier_env *env)
{
	struct bpf_verifier_state *state = &env->cur_state;
	struct bpf_reg_state *regs = state->regs, *reg;
	int i;

	for (i = 0; i < MAX_BPF_REG; i++)
		if (regs[i].type == PTR_TO_PACKET ||
		    regs[i].type == PTR_TO_PACKET_END)
			mark_reg_unknown_value(regs, i);

	for (i = 0; i < MAX_BPF_STACK; i += BPF_REG_SIZE) {
		if (state->stack_slot_type[i] != STACK_SPILL)
			continue;
		reg = &state->spilled_regs[i / BPF_REG_SIZE];
		if (reg->type != PTR_TO_PACKET &&
		    reg->type != PTR_TO_PACKET_END)
			continue;
		reg->type = UNKNOWN_VALUE;
		reg->imm = 0;
	}
}
