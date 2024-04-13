static bool arg_type_is_alloc_size(enum bpf_arg_type type)
{
	return type == ARG_CONST_ALLOC_SIZE_OR_ZERO;
}