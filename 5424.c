static int copy_reference_state(struct bpf_func_state *dst, const struct bpf_func_state *src)
{
	dst->refs = copy_array(dst->refs, src->refs, src->acquired_refs,
			       sizeof(struct bpf_reference_state), GFP_KERNEL);
	if (!dst->refs)
		return -ENOMEM;

	dst->acquired_refs = src->acquired_refs;
	return 0;
}