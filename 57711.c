static int ext_analyzer_insn_hook(struct bpf_verifier_env *env,
				  int insn_idx, int prev_insn_idx)
{
	if (!env->analyzer_ops || !env->analyzer_ops->insn_hook)
		return 0;

	return env->analyzer_ops->insn_hook(env, insn_idx, prev_insn_idx);
}
