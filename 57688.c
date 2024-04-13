static int check_ctx_access(struct bpf_verifier_env *env, int off, int size,
			    enum bpf_access_type t, enum bpf_reg_type *reg_type)
{
	/* for analyzer ctx accesses are already validated and converted */
	if (env->analyzer_ops)
		return 0;

	if (env->prog->aux->ops->is_valid_access &&
	    env->prog->aux->ops->is_valid_access(off, size, t, reg_type)) {
		/* remember the offset of last byte accessed in ctx */
		if (env->prog->aux->max_ctx_offset < off + size)
			env->prog->aux->max_ctx_offset = off + size;
		return 0;
	}

	verbose("invalid bpf_context access off=%d size=%d\n", off, size);
	return -EACCES;
}
