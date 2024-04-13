static int check_kfunc_call(struct bpf_verifier_env *env, struct bpf_insn *insn)
{
	const struct btf_type *t, *func, *func_proto, *ptr_type;
	struct bpf_reg_state *regs = cur_regs(env);
	const char *func_name, *ptr_type_name;
	u32 i, nargs, func_id, ptr_type_id;
	struct module *btf_mod = NULL;
	const struct btf_param *args;
	struct btf *desc_btf;
	int err;

	/* skip for now, but return error when we find this in fixup_kfunc_call */
	if (!insn->imm)
		return 0;

	desc_btf = find_kfunc_desc_btf(env, insn->imm, insn->off, &btf_mod);
	if (IS_ERR(desc_btf))
		return PTR_ERR(desc_btf);

	func_id = insn->imm;
	func = btf_type_by_id(desc_btf, func_id);
	func_name = btf_name_by_offset(desc_btf, func->name_off);
	func_proto = btf_type_by_id(desc_btf, func->type);

	if (!env->ops->check_kfunc_call ||
	    !env->ops->check_kfunc_call(func_id, btf_mod)) {
		verbose(env, "calling kernel function %s is not allowed\n",
			func_name);
		return -EACCES;
	}

	/* Check the arguments */
	err = btf_check_kfunc_arg_match(env, desc_btf, func_id, regs);
	if (err)
		return err;

	for (i = 0; i < CALLER_SAVED_REGS; i++)
		mark_reg_not_init(env, regs, caller_saved[i]);

	/* Check return type */
	t = btf_type_skip_modifiers(desc_btf, func_proto->type, NULL);
	if (btf_type_is_scalar(t)) {
		mark_reg_unknown(env, regs, BPF_REG_0);
		mark_btf_func_reg_size(env, BPF_REG_0, t->size);
	} else if (btf_type_is_ptr(t)) {
		ptr_type = btf_type_skip_modifiers(desc_btf, t->type,
						   &ptr_type_id);
		if (!btf_type_is_struct(ptr_type)) {
			ptr_type_name = btf_name_by_offset(desc_btf,
							   ptr_type->name_off);
			verbose(env, "kernel function %s returns pointer type %s %s is not supported\n",
				func_name, btf_type_str(ptr_type),
				ptr_type_name);
			return -EINVAL;
		}
		mark_reg_known_zero(env, regs, BPF_REG_0);
		regs[BPF_REG_0].btf = desc_btf;
		regs[BPF_REG_0].type = PTR_TO_BTF_ID;
		regs[BPF_REG_0].btf_id = ptr_type_id;
		mark_btf_func_reg_size(env, BPF_REG_0, sizeof(void *));
	} /* else { add_kfunc_call() ensures it is btf_type_is_void(t) } */

	nargs = btf_type_vlen(func_proto);
	args = (const struct btf_param *)(func_proto + 1);
	for (i = 0; i < nargs; i++) {
		u32 regno = i + 1;

		t = btf_type_skip_modifiers(desc_btf, args[i].type, NULL);
		if (btf_type_is_ptr(t))
			mark_btf_func_reg_size(env, regno, sizeof(void *));
		else
			/* scalar. ensured by btf_check_kfunc_arg_match() */
			mark_btf_func_reg_size(env, regno, t->size);
	}

	return 0;
}