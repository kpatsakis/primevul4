static int visit_func_call_insn(int t, int insn_cnt,
				struct bpf_insn *insns,
				struct bpf_verifier_env *env,
				bool visit_callee)
{
	int ret;

	ret = push_insn(t, t + 1, FALLTHROUGH, env, false);
	if (ret)
		return ret;

	if (t + 1 < insn_cnt)
		init_explored_state(env, t + 1);
	if (visit_callee) {
		init_explored_state(env, t);
		ret = push_insn(t, t + insns[t].imm + 1, BRANCH, env,
				/* It's ok to allow recursion from CFG point of
				 * view. __check_func_call() will do the actual
				 * check.
				 */
				bpf_pseudo_func(insns + t));
	}
	return ret;
}