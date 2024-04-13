static int check_func_call(struct bpf_verifier_env *env, struct bpf_insn *insn,
			   int *insn_idx)
{
	int subprog, target_insn;

	target_insn = *insn_idx + insn->imm + 1;
	subprog = find_subprog(env, target_insn);
	if (subprog < 0) {
		verbose(env, "verifier bug. No program starts at insn %d\n",
			target_insn);
		return -EFAULT;
	}

	return __check_func_call(env, insn, insn_idx, subprog, set_callee_state);
}