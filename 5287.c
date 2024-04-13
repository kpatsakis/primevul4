static void __mark_reg_unknown(const struct bpf_verifier_env *env,
			       struct bpf_reg_state *reg)
{
	/*
	 * Clear type, id, off, and union(map_ptr, range) and
	 * padding between 'type' and union
	 */
	memset(reg, 0, offsetof(struct bpf_reg_state, var_off));
	reg->type = SCALAR_VALUE;
	reg->var_off = tnum_unknown;
	reg->frameno = 0;
	reg->precise = env->subprog_cnt > 1 || !env->bpf_capable;
	__mark_reg_unbounded(reg);
}