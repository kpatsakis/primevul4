static const char *reg_type_str(struct bpf_verifier_env *env,
				enum bpf_reg_type type)
{
	char postfix[16] = {0}, prefix[16] = {0};
	static const char * const str[] = {
		[NOT_INIT]		= "?",
		[SCALAR_VALUE]		= "inv",
		[PTR_TO_CTX]		= "ctx",
		[CONST_PTR_TO_MAP]	= "map_ptr",
		[PTR_TO_MAP_VALUE]	= "map_value",
		[PTR_TO_STACK]		= "fp",
		[PTR_TO_PACKET]		= "pkt",
		[PTR_TO_PACKET_META]	= "pkt_meta",
		[PTR_TO_PACKET_END]	= "pkt_end",
		[PTR_TO_FLOW_KEYS]	= "flow_keys",
		[PTR_TO_SOCKET]		= "sock",
		[PTR_TO_SOCK_COMMON]	= "sock_common",
		[PTR_TO_TCP_SOCK]	= "tcp_sock",
		[PTR_TO_TP_BUFFER]	= "tp_buffer",
		[PTR_TO_XDP_SOCK]	= "xdp_sock",
		[PTR_TO_BTF_ID]		= "ptr_",
		[PTR_TO_PERCPU_BTF_ID]	= "percpu_ptr_",
		[PTR_TO_MEM]		= "mem",
		[PTR_TO_BUF]		= "buf",
		[PTR_TO_FUNC]		= "func",
		[PTR_TO_MAP_KEY]	= "map_key",
	};

	if (type & PTR_MAYBE_NULL) {
		if (base_type(type) == PTR_TO_BTF_ID ||
		    base_type(type) == PTR_TO_PERCPU_BTF_ID)
			strncpy(postfix, "or_null_", 16);
		else
			strncpy(postfix, "_or_null", 16);
	}

	if (type & MEM_RDONLY)
		strncpy(prefix, "rdonly_", 16);

	snprintf(env->type_str_buf, TYPE_STR_BUF_LEN, "%s%s%s",
		 prefix, str[base_type(type)], postfix);
	return env->type_str_buf;
}