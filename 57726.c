static void print_verifier_state(struct bpf_verifier_state *state)
{
	struct bpf_reg_state *reg;
	enum bpf_reg_type t;
	int i;

	for (i = 0; i < MAX_BPF_REG; i++) {
		reg = &state->regs[i];
		t = reg->type;
		if (t == NOT_INIT)
			continue;
		verbose(" R%d=%s", i, reg_type_str[t]);
		if (t == CONST_IMM || t == PTR_TO_STACK)
			verbose("%lld", reg->imm);
		else if (t == PTR_TO_PACKET)
			verbose("(id=%d,off=%d,r=%d)",
				reg->id, reg->off, reg->range);
		else if (t == UNKNOWN_VALUE && reg->imm)
			verbose("%lld", reg->imm);
		else if (t == CONST_PTR_TO_MAP || t == PTR_TO_MAP_VALUE ||
			 t == PTR_TO_MAP_VALUE_OR_NULL ||
			 t == PTR_TO_MAP_VALUE_ADJ)
			verbose("(ks=%d,vs=%d,id=%u)",
				reg->map_ptr->key_size,
				reg->map_ptr->value_size,
				reg->id);
		if (reg->min_value != BPF_REGISTER_MIN_RANGE)
			verbose(",min_value=%lld",
				(long long)reg->min_value);
		if (reg->max_value != BPF_REGISTER_MAX_RANGE)
			verbose(",max_value=%llu",
				(unsigned long long)reg->max_value);
	}
	for (i = 0; i < MAX_BPF_STACK; i += BPF_REG_SIZE) {
		if (state->stack_slot_type[i] == STACK_SPILL)
			verbose(" fp%d=%s", -MAX_BPF_STACK + i,
				reg_type_str[state->spilled_regs[i / BPF_REG_SIZE].type]);
	}
	verbose("\n");
}
