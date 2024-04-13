static bool may_be_acquire_function(enum bpf_func_id func_id)
{
	return func_id == BPF_FUNC_sk_lookup_tcp ||
		func_id == BPF_FUNC_sk_lookup_udp ||
		func_id == BPF_FUNC_skc_lookup_tcp ||
		func_id == BPF_FUNC_map_lookup_elem ||
	        func_id == BPF_FUNC_ringbuf_reserve;
}