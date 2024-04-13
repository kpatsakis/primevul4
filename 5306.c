static int copy_func_state(struct bpf_func_state *dst,
			   const struct bpf_func_state *src)
{
	int err;

	memcpy(dst, src, offsetof(struct bpf_func_state, acquired_refs));
	err = copy_reference_state(dst, src);
	if (err)
		return err;
	return copy_stack_state(dst, src);
}