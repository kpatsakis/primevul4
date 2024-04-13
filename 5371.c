static bool insn_has_def32(struct bpf_verifier_env *env, struct bpf_insn *insn)
{
	int dst_reg = insn_def_regno(insn);

	if (dst_reg == -1)
		return false;

	return !is_reg64(env, insn, dst_reg, NULL, DST_OP);
}