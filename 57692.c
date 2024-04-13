static int check_ld_imm(struct bpf_verifier_env *env, struct bpf_insn *insn)
{
	struct bpf_reg_state *regs = env->cur_state.regs;
	int err;

	if (BPF_SIZE(insn->code) != BPF_DW) {
		verbose("invalid BPF_LD_IMM insn\n");
		return -EINVAL;
	}
	if (insn->off != 0) {
		verbose("BPF_LD_IMM64 uses reserved fields\n");
		return -EINVAL;
	}

	err = check_reg_arg(regs, insn->dst_reg, DST_OP);
	if (err)
		return err;

	if (insn->src_reg == 0) {
		u64 imm = ((u64)(insn + 1)->imm << 32) | (u32)insn->imm;

		regs[insn->dst_reg].type = CONST_IMM;
		regs[insn->dst_reg].imm = imm;
		return 0;
	}

	/* replace_map_fd_with_map_ptr() should have caught bad ld_imm64 */
	BUG_ON(insn->src_reg != BPF_PSEUDO_MAP_FD);

	regs[insn->dst_reg].type = CONST_PTR_TO_MAP;
	regs[insn->dst_reg].map_ptr = ld_imm64_to_map_ptr(insn);
	return 0;
}
