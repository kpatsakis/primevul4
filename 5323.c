static int check_helper_mem_access(struct bpf_verifier_env *env, int regno,
				   int access_size, bool zero_size_allowed,
				   struct bpf_call_arg_meta *meta)
{
	struct bpf_reg_state *regs = cur_regs(env), *reg = &regs[regno];
	const char *buf_info;
	u32 *max_access;

	switch (base_type(reg->type)) {
	case PTR_TO_PACKET:
	case PTR_TO_PACKET_META:
		return check_packet_access(env, regno, reg->off, access_size,
					   zero_size_allowed);
	case PTR_TO_MAP_KEY:
		return check_mem_region_access(env, regno, reg->off, access_size,
					       reg->map_ptr->key_size, false);
	case PTR_TO_MAP_VALUE:
		if (check_map_access_type(env, regno, reg->off, access_size,
					  meta && meta->raw_mode ? BPF_WRITE :
					  BPF_READ))
			return -EACCES;
		return check_map_access(env, regno, reg->off, access_size,
					zero_size_allowed);
	case PTR_TO_MEM:
		return check_mem_region_access(env, regno, reg->off,
					       access_size, reg->mem_size,
					       zero_size_allowed);
	case PTR_TO_BUF:
		if (type_is_rdonly_mem(reg->type)) {
			if (meta && meta->raw_mode)
				return -EACCES;

			buf_info = "rdonly";
			max_access = &env->prog->aux->max_rdonly_access;
		} else {
			buf_info = "rdwr";
			max_access = &env->prog->aux->max_rdwr_access;
		}
		return check_buffer_access(env, reg, regno, reg->off,
					   access_size, zero_size_allowed,
					   buf_info, max_access);
	case PTR_TO_STACK:
		return check_stack_range_initialized(
				env,
				regno, reg->off, access_size,
				zero_size_allowed, ACCESS_HELPER, meta);
	default: /* scalar_value or invalid ptr */
		/* Allow zero-byte read from NULL, regardless of pointer type */
		if (zero_size_allowed && access_size == 0 &&
		    register_is_null(reg))
			return 0;

		verbose(env, "R%d type=%s ", regno,
			reg_type_str(env, reg->type));
		verbose(env, "expected=%s\n", reg_type_str(env, PTR_TO_STACK));
		return -EACCES;
	}
}