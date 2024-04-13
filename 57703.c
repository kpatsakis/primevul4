static int check_stack_read(struct bpf_verifier_state *state, int off, int size,
			    int value_regno)
{
	u8 *slot_type;
	int i;

	slot_type = &state->stack_slot_type[MAX_BPF_STACK + off];

	if (slot_type[0] == STACK_SPILL) {
		if (size != BPF_REG_SIZE) {
			verbose("invalid size of register spill\n");
			return -EACCES;
		}
		for (i = 1; i < BPF_REG_SIZE; i++) {
			if (slot_type[i] != STACK_SPILL) {
				verbose("corrupted spill memory\n");
				return -EACCES;
			}
		}

		if (value_regno >= 0)
			/* restore register state from stack */
			state->regs[value_regno] =
				state->spilled_regs[(MAX_BPF_STACK + off) / BPF_REG_SIZE];
		return 0;
	} else {
		for (i = 0; i < size; i++) {
			if (slot_type[i] != STACK_MISC) {
				verbose("invalid read from stack off %d+%d size %d\n",
					off, i, size);
				return -EACCES;
			}
		}
		if (value_regno >= 0)
			/* have read misc data from the stack */
			mark_reg_unknown_value_and_range(state->regs,
							 value_regno);
		return 0;
	}
}
