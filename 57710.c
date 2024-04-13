static int evaluate_reg_imm_alu(struct bpf_verifier_env *env,
				struct bpf_insn *insn)
{
	struct bpf_reg_state *regs = env->cur_state.regs;
	struct bpf_reg_state *dst_reg = &regs[insn->dst_reg];
	struct bpf_reg_state *src_reg = &regs[insn->src_reg];
	u8 opcode = BPF_OP(insn->code);
	u64 dst_imm = dst_reg->imm;

	/* dst_reg->type == CONST_IMM here. Simulate execution of insns
	 * containing ALU ops. Don't care about overflow or negative
	 * values, just add/sub/... them; registers are in u64.
	 */
	if (opcode == BPF_ADD && BPF_SRC(insn->code) == BPF_K) {
		dst_imm += insn->imm;
	} else if (opcode == BPF_ADD && BPF_SRC(insn->code) == BPF_X &&
		   src_reg->type == CONST_IMM) {
		dst_imm += src_reg->imm;
	} else if (opcode == BPF_SUB && BPF_SRC(insn->code) == BPF_K) {
		dst_imm -= insn->imm;
	} else if (opcode == BPF_SUB && BPF_SRC(insn->code) == BPF_X &&
		   src_reg->type == CONST_IMM) {
		dst_imm -= src_reg->imm;
	} else if (opcode == BPF_MUL && BPF_SRC(insn->code) == BPF_K) {
		dst_imm *= insn->imm;
	} else if (opcode == BPF_MUL && BPF_SRC(insn->code) == BPF_X &&
		   src_reg->type == CONST_IMM) {
		dst_imm *= src_reg->imm;
	} else if (opcode == BPF_OR && BPF_SRC(insn->code) == BPF_K) {
		dst_imm |= insn->imm;
	} else if (opcode == BPF_OR && BPF_SRC(insn->code) == BPF_X &&
		   src_reg->type == CONST_IMM) {
		dst_imm |= src_reg->imm;
	} else if (opcode == BPF_AND && BPF_SRC(insn->code) == BPF_K) {
		dst_imm &= insn->imm;
	} else if (opcode == BPF_AND && BPF_SRC(insn->code) == BPF_X &&
		   src_reg->type == CONST_IMM) {
		dst_imm &= src_reg->imm;
	} else if (opcode == BPF_RSH && BPF_SRC(insn->code) == BPF_K) {
		dst_imm >>= insn->imm;
	} else if (opcode == BPF_RSH && BPF_SRC(insn->code) == BPF_X &&
		   src_reg->type == CONST_IMM) {
		dst_imm >>= src_reg->imm;
	} else if (opcode == BPF_LSH && BPF_SRC(insn->code) == BPF_K) {
		dst_imm <<= insn->imm;
	} else if (opcode == BPF_LSH && BPF_SRC(insn->code) == BPF_X &&
		   src_reg->type == CONST_IMM) {
		dst_imm <<= src_reg->imm;
	} else {
		mark_reg_unknown_value(regs, insn->dst_reg);
		goto out;
	}

	dst_reg->imm = dst_imm;
out:
	return 0;
}
