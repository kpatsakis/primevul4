static int copy_stack_state(struct bpf_func_state *dst, const struct bpf_func_state *src)
{
	size_t n = src->allocated_stack / BPF_REG_SIZE;

	dst->stack = copy_array(dst->stack, src->stack, n, sizeof(struct bpf_stack_state),
				GFP_KERNEL);
	if (!dst->stack)
		return -ENOMEM;

	dst->allocated_stack = src->allocated_stack;
	return 0;
}