static int sanitize_check_bounds(struct bpf_verifier_env *env,
				 const struct bpf_insn *insn,
				 const struct bpf_reg_state *dst_reg)
{
	u32 dst = insn->dst_reg;

	/* For unprivileged we require that resulting offset must be in bounds
	 * in order to be able to sanitize access later on.
	 */
	if (env->bypass_spec_v1)
		return 0;

	switch (dst_reg->type) {
	case PTR_TO_STACK:
		if (check_stack_access_for_ptr_arithmetic(env, dst, dst_reg,
					dst_reg->off + dst_reg->var_off.value))
			return -EACCES;
		break;
	case PTR_TO_MAP_VALUE:
		if (check_map_access(env, dst, dst_reg->off, 1, false)) {
			verbose(env, "R%d pointer arithmetic of map value goes out of range, "
				"prohibited for !root\n", dst);
			return -EACCES;
		}
		break;
	default:
		break;
	}

	return 0;
}