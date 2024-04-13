static int check_func_proto(const struct bpf_func_proto *fn, int func_id)
{
	return check_raw_mode_ok(fn) &&
	       check_arg_pair_ok(fn) &&
	       check_btf_id_ok(fn) &&
	       check_refcount_ok(fn, func_id) ? 0 : -EINVAL;
}