static int check_map_access_adj(struct bpf_verifier_env *env, u32 regno,
				int off, int size)
{
	struct bpf_verifier_state *state = &env->cur_state;
	struct bpf_reg_state *reg = &state->regs[regno];
	int err;

	/* We adjusted the register to this map value, so we
	 * need to change off and size to min_value and max_value
	 * respectively to make sure our theoretical access will be
	 * safe.
	 */
	if (log_level)
		print_verifier_state(state);
	env->varlen_map_value_access = true;
	/* The minimum value is only important with signed
	 * comparisons where we can't assume the floor of a
	 * value is 0.  If we are using signed variables for our
	 * index'es we need to make sure that whatever we use
	 * will have a set floor within our range.
	 */
	if (reg->min_value < 0) {
		verbose("R%d min value is negative, either use unsigned index or do a if (index >=0) check.\n",
			regno);
		return -EACCES;
	}
	err = check_map_access(env, regno, reg->min_value + off, size);
	if (err) {
		verbose("R%d min value is outside of the array range\n",
			regno);
		return err;
	}

	/* If we haven't set a max value then we need to bail
	 * since we can't be sure we won't do bad things.
	 */
	if (reg->max_value == BPF_REGISTER_MAX_RANGE) {
		verbose("R%d unbounded memory access, make sure to bounds check any array access into a map\n",
			regno);
		return -EACCES;
	}
	return check_map_access(env, regno, reg->max_value + off, size);
}
