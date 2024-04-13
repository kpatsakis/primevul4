static int check_func_arg(struct verifier_env *env, u32 regno,
			  enum bpf_arg_type arg_type, struct bpf_map **mapp)
{
	struct reg_state *reg = env->cur_state.regs + regno;
	enum bpf_reg_type expected_type;
	int err = 0;

	if (arg_type == ARG_DONTCARE)
		return 0;

	if (reg->type == NOT_INIT) {
		verbose("R%d !read_ok\n", regno);
		return -EACCES;
	}

	if (arg_type == ARG_ANYTHING) {
		if (is_pointer_value(env, regno)) {
			verbose("R%d leaks addr into helper function\n", regno);
			return -EACCES;
		}
		return 0;
	}

	if (arg_type == ARG_PTR_TO_STACK || arg_type == ARG_PTR_TO_MAP_KEY ||
	    arg_type == ARG_PTR_TO_MAP_VALUE) {
		expected_type = PTR_TO_STACK;
	} else if (arg_type == ARG_CONST_STACK_SIZE) {
		expected_type = CONST_IMM;
	} else if (arg_type == ARG_CONST_MAP_PTR) {
		expected_type = CONST_PTR_TO_MAP;
	} else if (arg_type == ARG_PTR_TO_CTX) {
		expected_type = PTR_TO_CTX;
	} else {
		verbose("unsupported arg_type %d\n", arg_type);
		return -EFAULT;
	}

	if (reg->type != expected_type) {
		verbose("R%d type=%s expected=%s\n", regno,
			reg_type_str[reg->type], reg_type_str[expected_type]);
		return -EACCES;
	}

	if (arg_type == ARG_CONST_MAP_PTR) {
		/* bpf_map_xxx(map_ptr) call: remember that map_ptr */
		*mapp = reg->map_ptr;

	} else if (arg_type == ARG_PTR_TO_MAP_KEY) {
		/* bpf_map_xxx(..., map_ptr, ..., key) call:
		 * check that [key, key + map->key_size) are within
		 * stack limits and initialized
		 */
		if (!*mapp) {
			/* in function declaration map_ptr must come before
			 * map_key, so that it's verified and known before
			 * we have to check map_key here. Otherwise it means
			 * that kernel subsystem misconfigured verifier
			 */
			verbose("invalid map_ptr to access map->key\n");
			return -EACCES;
		}
		err = check_stack_boundary(env, regno, (*mapp)->key_size);

	} else if (arg_type == ARG_PTR_TO_MAP_VALUE) {
		/* bpf_map_xxx(..., map_ptr, ..., value) call:
		 * check [value, value + map->value_size) validity
		 */
		if (!*mapp) {
			/* kernel subsystem misconfigured verifier */
			verbose("invalid map_ptr to access map->value\n");
			return -EACCES;
		}
		err = check_stack_boundary(env, regno, (*mapp)->value_size);

	} else if (arg_type == ARG_CONST_STACK_SIZE) {
		/* bpf_xxx(..., buf, len) call will access 'len' bytes
		 * from stack pointer 'buf'. Check it
		 * note: regno == len, regno - 1 == buf
		 */
		if (regno == 0) {
			/* kernel subsystem misconfigured verifier */
			verbose("ARG_CONST_STACK_SIZE cannot be first argument\n");
			return -EACCES;
		}
		err = check_stack_boundary(env, regno - 1, reg->imm);
	}

	return err;
}
