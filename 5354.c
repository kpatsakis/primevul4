static void save_register_state(struct bpf_func_state *state,
				int spi, struct bpf_reg_state *reg,
				int size)
{
	int i;

	state->stack[spi].spilled_ptr = *reg;
	if (size == BPF_REG_SIZE)
		state->stack[spi].spilled_ptr.live |= REG_LIVE_WRITTEN;

	for (i = BPF_REG_SIZE; i > BPF_REG_SIZE - size; i--)
		state->stack[spi].slot_type[i - 1] = STACK_SPILL;

	/* size < 8 bytes spill */
	for (; i; i--)
		scrub_spilled_slot(&state->stack[spi].slot_type[i - 1]);
}