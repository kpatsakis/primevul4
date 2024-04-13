static bool func_states_equal(struct bpf_verifier_env *env, struct bpf_func_state *old,
			      struct bpf_func_state *cur)
{
	int i;

	memset(env->idmap_scratch, 0, sizeof(env->idmap_scratch));
	for (i = 0; i < MAX_BPF_REG; i++)
		if (!regsafe(env, &old->regs[i], &cur->regs[i],
			     env->idmap_scratch))
			return false;

	if (!stacksafe(env, old, cur, env->idmap_scratch))
		return false;

	if (!refsafe(old, cur))
		return false;

	return true;
}