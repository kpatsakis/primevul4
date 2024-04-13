static int check_stack_read(struct bpf_verifier_env *env,
			    int ptr_regno, int off, int size,
			    int dst_regno)
{
	struct bpf_reg_state *reg = reg_state(env, ptr_regno);
	struct bpf_func_state *state = func(env, reg);
	int err;
	/* Some accesses are only permitted with a static offset. */
	bool var_off = !tnum_is_const(reg->var_off);

	/* The offset is required to be static when reads don't go to a
	 * register, in order to not leak pointers (see
	 * check_stack_read_fixed_off).
	 */
	if (dst_regno < 0 && var_off) {
		char tn_buf[48];

		tnum_strn(tn_buf, sizeof(tn_buf), reg->var_off);
		verbose(env, "variable offset stack pointer cannot be passed into helper function; var_off=%s off=%d size=%d\n",
			tn_buf, off, size);
		return -EACCES;
	}
	/* Variable offset is prohibited for unprivileged mode for simplicity
	 * since it requires corresponding support in Spectre masking for stack
	 * ALU. See also retrieve_ptr_limit().
	 */
	if (!env->bypass_spec_v1 && var_off) {
		char tn_buf[48];

		tnum_strn(tn_buf, sizeof(tn_buf), reg->var_off);
		verbose(env, "R%d variable offset stack access prohibited for !root, var_off=%s\n",
				ptr_regno, tn_buf);
		return -EACCES;
	}

	if (!var_off) {
		off += reg->var_off.value;
		err = check_stack_read_fixed_off(env, state, off, size,
						 dst_regno);
	} else {
		/* Variable offset stack reads need more conservative handling
		 * than fixed offset ones. Note that dst_regno >= 0 on this
		 * branch.
		 */
		err = check_stack_read_var_off(env, ptr_regno, off, size,
					       dst_regno);
	}
	return err;
}