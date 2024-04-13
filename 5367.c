static int insn_def_regno(const struct bpf_insn *insn)
{
	switch (BPF_CLASS(insn->code)) {
	case BPF_JMP:
	case BPF_JMP32:
	case BPF_ST:
		return -1;
	case BPF_STX:
		if (BPF_MODE(insn->code) == BPF_ATOMIC &&
		    (insn->imm & BPF_FETCH)) {
			if (insn->imm == BPF_CMPXCHG)
				return BPF_REG_0;
			else
				return insn->src_reg;
		} else {
			return -1;
		}
	default:
		return insn->dst_reg;
	}
}