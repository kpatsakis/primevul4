static bool is_release_function(enum bpf_func_id func_id)
{
	return func_id == BPF_FUNC_sk_release ||
	       func_id == BPF_FUNC_ringbuf_submit ||
	       func_id == BPF_FUNC_ringbuf_discard;
}