static bool arg_type_is_mem_ptr(enum bpf_arg_type type)
{
	return base_type(type) == ARG_PTR_TO_MEM ||
	       base_type(type) == ARG_PTR_TO_UNINIT_MEM;
}