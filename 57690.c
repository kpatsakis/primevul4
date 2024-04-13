static int check_helper_mem_access(struct bpf_verifier_env *env, int regno,
				   int access_size, bool zero_size_allowed,
				   struct bpf_call_arg_meta *meta)
{
	struct bpf_reg_state *regs = env->cur_state.regs;

	switch (regs[regno].type) {
	case PTR_TO_PACKET:
		return check_packet_access(env, regno, 0, access_size);
	case PTR_TO_MAP_VALUE:
		return check_map_access(env, regno, 0, access_size);
	case PTR_TO_MAP_VALUE_ADJ:
		return check_map_access_adj(env, regno, 0, access_size);
	default: /* const_imm|ptr_to_stack or invalid ptr */
		return check_stack_boundary(env, regno, access_size,
					    zero_size_allowed, meta);
	}
}
