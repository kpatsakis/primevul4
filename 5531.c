static int resolve_map_arg_type(struct bpf_verifier_env *env,
				 const struct bpf_call_arg_meta *meta,
				 enum bpf_arg_type *arg_type)
{
	if (!meta->map_ptr) {
		/* kernel subsystem misconfigured verifier */
		verbose(env, "invalid map_ptr to access map->type\n");
		return -EACCES;
	}

	switch (meta->map_ptr->map_type) {
	case BPF_MAP_TYPE_SOCKMAP:
	case BPF_MAP_TYPE_SOCKHASH:
		if (*arg_type == ARG_PTR_TO_MAP_VALUE) {
			*arg_type = ARG_PTR_TO_BTF_ID_SOCK_COMMON;
		} else {
			verbose(env, "invalid arg_type for sockmap/sockhash\n");
			return -EINVAL;
		}
		break;
	case BPF_MAP_TYPE_BLOOM_FILTER:
		if (meta->func_id == BPF_FUNC_map_peek_elem)
			*arg_type = ARG_PTR_TO_MAP_VALUE;
		break;
	default:
		break;
	}
	return 0;
}