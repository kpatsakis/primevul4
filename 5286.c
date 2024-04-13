static void reg_set_min_max_inv(struct bpf_reg_state *true_reg,
				struct bpf_reg_state *false_reg,
				u64 val, u32 val32,
				u8 opcode, bool is_jmp32)
{
	opcode = flip_opcode(opcode);
	/* This uses zero as "not present in table"; luckily the zero opcode,
	 * BPF_JA, can't get here.
	 */
	if (opcode)
		reg_set_min_max(true_reg, false_reg, val, val32, opcode, is_jmp32);
}