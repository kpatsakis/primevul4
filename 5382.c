static int add_kfunc_call(struct bpf_verifier_env *env, u32 func_id, s16 offset)
{
	const struct btf_type *func, *func_proto;
	struct bpf_kfunc_btf_tab *btf_tab;
	struct bpf_kfunc_desc_tab *tab;
	struct bpf_prog_aux *prog_aux;
	struct bpf_kfunc_desc *desc;
	const char *func_name;
	struct btf *desc_btf;
	unsigned long addr;
	int err;

	prog_aux = env->prog->aux;
	tab = prog_aux->kfunc_tab;
	btf_tab = prog_aux->kfunc_btf_tab;
	if (!tab) {
		if (!btf_vmlinux) {
			verbose(env, "calling kernel function is not supported without CONFIG_DEBUG_INFO_BTF\n");
			return -ENOTSUPP;
		}

		if (!env->prog->jit_requested) {
			verbose(env, "JIT is required for calling kernel function\n");
			return -ENOTSUPP;
		}

		if (!bpf_jit_supports_kfunc_call()) {
			verbose(env, "JIT does not support calling kernel function\n");
			return -ENOTSUPP;
		}

		if (!env->prog->gpl_compatible) {
			verbose(env, "cannot call kernel function from non-GPL compatible program\n");
			return -EINVAL;
		}

		tab = kzalloc(sizeof(*tab), GFP_KERNEL);
		if (!tab)
			return -ENOMEM;
		prog_aux->kfunc_tab = tab;
	}

	/* func_id == 0 is always invalid, but instead of returning an error, be
	 * conservative and wait until the code elimination pass before returning
	 * error, so that invalid calls that get pruned out can be in BPF programs
	 * loaded from userspace.  It is also required that offset be untouched
	 * for such calls.
	 */
	if (!func_id && !offset)
		return 0;

	if (!btf_tab && offset) {
		btf_tab = kzalloc(sizeof(*btf_tab), GFP_KERNEL);
		if (!btf_tab)
			return -ENOMEM;
		prog_aux->kfunc_btf_tab = btf_tab;
	}

	desc_btf = find_kfunc_desc_btf(env, func_id, offset, NULL);
	if (IS_ERR(desc_btf)) {
		verbose(env, "failed to find BTF for kernel function\n");
		return PTR_ERR(desc_btf);
	}

	if (find_kfunc_desc(env->prog, func_id, offset))
		return 0;

	if (tab->nr_descs == MAX_KFUNC_DESCS) {
		verbose(env, "too many different kernel function calls\n");
		return -E2BIG;
	}

	func = btf_type_by_id(desc_btf, func_id);
	if (!func || !btf_type_is_func(func)) {
		verbose(env, "kernel btf_id %u is not a function\n",
			func_id);
		return -EINVAL;
	}
	func_proto = btf_type_by_id(desc_btf, func->type);
	if (!func_proto || !btf_type_is_func_proto(func_proto)) {
		verbose(env, "kernel function btf_id %u does not have a valid func_proto\n",
			func_id);
		return -EINVAL;
	}

	func_name = btf_name_by_offset(desc_btf, func->name_off);
	addr = kallsyms_lookup_name(func_name);
	if (!addr) {
		verbose(env, "cannot find address for kernel function %s\n",
			func_name);
		return -EINVAL;
	}

	desc = &tab->descs[tab->nr_descs++];
	desc->func_id = func_id;
	desc->imm = BPF_CALL_IMM(addr);
	desc->offset = offset;
	err = btf_distill_func_proto(&env->log, desc_btf,
				     func_proto, func_name,
				     &desc->func_model);
	if (!err)
		sort(tab->descs, tab->nr_descs, sizeof(tab->descs[0]),
		     kfunc_desc_cmp_by_id_off, NULL);
	return err;
}