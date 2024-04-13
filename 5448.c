static int check_get_func_ip(struct bpf_verifier_env *env)
{
	enum bpf_prog_type type = resolve_prog_type(env->prog);
	int func_id = BPF_FUNC_get_func_ip;

	if (type == BPF_PROG_TYPE_TRACING) {
		if (!bpf_prog_has_trampoline(env->prog)) {
			verbose(env, "func %s#%d supported only for fentry/fexit/fmod_ret programs\n",
				func_id_name(func_id), func_id);
			return -ENOTSUPP;
		}
		return 0;
	} else if (type == BPF_PROG_TYPE_KPROBE) {
		return 0;
	}

	verbose(env, "func %s#%d not supported for program type %d\n",
		func_id_name(func_id), func_id, type);
	return -ENOTSUPP;
}