static int is_branch_taken(struct bpf_reg_state *reg, u64 val, u8 opcode,
			   bool is_jmp32)
{
	if (__is_pointer_value(false, reg)) {
		if (!reg_type_not_null(reg->type))
			return -1;

		/* If pointer is valid tests against zero will fail so we can
		 * use this to direct branch taken.
		 */
		if (val != 0)
			return -1;

		switch (opcode) {
		case BPF_JEQ:
			return 0;
		case BPF_JNE:
			return 1;
		default:
			return -1;
		}
	}

	if (is_jmp32)
		return is_branch32_taken(reg, val, opcode);
	return is_branch64_taken(reg, val, opcode);
}