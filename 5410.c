static int check_stack_read_var_off(struct bpf_verifier_env *env,
				    int ptr_regno, int off, int size, int dst_regno)
{
	/* The state of the source register. */
	struct bpf_reg_state *reg = reg_state(env, ptr_regno);
	struct bpf_func_state *ptr_state = func(env, reg);
	int err;
	int min_off, max_off;

	/* Note that we pass a NULL meta, so raw access will not be permitted.
	 */
	err = check_stack_range_initialized(env, ptr_regno, off, size,
					    false, ACCESS_DIRECT, NULL);
	if (err)
		return err;

	min_off = reg->smin_value + off;
	max_off = reg->smax_value + off;
	mark_reg_stack_read(env, ptr_state, min_off, max_off + size, dst_regno);
	return 0;
}