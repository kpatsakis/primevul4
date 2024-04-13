static int check_attach_btf_id(struct bpf_verifier_env *env)
{
	struct bpf_prog *prog = env->prog;
	struct bpf_prog *tgt_prog = prog->aux->dst_prog;
	struct bpf_attach_target_info tgt_info = {};
	u32 btf_id = prog->aux->attach_btf_id;
	struct bpf_trampoline *tr;
	int ret;
	u64 key;

	if (prog->type == BPF_PROG_TYPE_SYSCALL) {
		if (prog->aux->sleepable)
			/* attach_btf_id checked to be zero already */
			return 0;
		verbose(env, "Syscall programs can only be sleepable\n");
		return -EINVAL;
	}

	if (prog->aux->sleepable && prog->type != BPF_PROG_TYPE_TRACING &&
	    prog->type != BPF_PROG_TYPE_LSM) {
		verbose(env, "Only fentry/fexit/fmod_ret and lsm programs can be sleepable\n");
		return -EINVAL;
	}

	if (prog->type == BPF_PROG_TYPE_STRUCT_OPS)
		return check_struct_ops_btf_id(env);

	if (prog->type != BPF_PROG_TYPE_TRACING &&
	    prog->type != BPF_PROG_TYPE_LSM &&
	    prog->type != BPF_PROG_TYPE_EXT)
		return 0;

	ret = bpf_check_attach_target(&env->log, prog, tgt_prog, btf_id, &tgt_info);
	if (ret)
		return ret;

	if (tgt_prog && prog->type == BPF_PROG_TYPE_EXT) {
		/* to make freplace equivalent to their targets, they need to
		 * inherit env->ops and expected_attach_type for the rest of the
		 * verification
		 */
		env->ops = bpf_verifier_ops[tgt_prog->type];
		prog->expected_attach_type = tgt_prog->expected_attach_type;
	}

	/* store info about the attachment target that will be used later */
	prog->aux->attach_func_proto = tgt_info.tgt_type;
	prog->aux->attach_func_name = tgt_info.tgt_name;

	if (tgt_prog) {
		prog->aux->saved_dst_prog_type = tgt_prog->type;
		prog->aux->saved_dst_attach_type = tgt_prog->expected_attach_type;
	}

	if (prog->expected_attach_type == BPF_TRACE_RAW_TP) {
		prog->aux->attach_btf_trace = true;
		return 0;
	} else if (prog->expected_attach_type == BPF_TRACE_ITER) {
		if (!bpf_iter_prog_supported(prog))
			return -EINVAL;
		return 0;
	}

	if (prog->type == BPF_PROG_TYPE_LSM) {
		ret = bpf_lsm_verify_prog(&env->log, prog);
		if (ret < 0)
			return ret;
	} else if (prog->type == BPF_PROG_TYPE_TRACING &&
		   btf_id_set_contains(&btf_id_deny, btf_id)) {
		return -EINVAL;
	}

	key = bpf_trampoline_compute_key(tgt_prog, prog->aux->attach_btf, btf_id);
	tr = bpf_trampoline_get(key, &tgt_info);
	if (!tr)
		return -ENOMEM;

	prog->aux->dst_trampoline = tr;
	return 0;
}