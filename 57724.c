static bool may_access_direct_pkt_data(struct bpf_verifier_env *env,
				       const struct bpf_call_arg_meta *meta,
				       enum bpf_access_type t)
{
	switch (env->prog->type) {
	case BPF_PROG_TYPE_LWT_IN:
	case BPF_PROG_TYPE_LWT_OUT:
		/* dst_input() and dst_output() can't write for now */
		if (t == BPF_WRITE)
			return false;
		/* fallthrough */
	case BPF_PROG_TYPE_SCHED_CLS:
	case BPF_PROG_TYPE_SCHED_ACT:
	case BPF_PROG_TYPE_XDP:
	case BPF_PROG_TYPE_LWT_XMIT:
		if (meta)
			return meta->pkt_access;

		env->seen_direct_write = true;
		return true;
	default:
		return false;
	}
}
