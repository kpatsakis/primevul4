static int check_ptr_to_btf_access(struct bpf_verifier_env *env,
				   struct bpf_reg_state *regs,
				   int regno, int off, int size,
				   enum bpf_access_type atype,
				   int value_regno)
{
	struct bpf_reg_state *reg = regs + regno;
	const struct btf_type *t = btf_type_by_id(reg->btf, reg->btf_id);
	const char *tname = btf_name_by_offset(reg->btf, t->name_off);
	u32 btf_id;
	int ret;

	if (off < 0) {
		verbose(env,
			"R%d is ptr_%s invalid negative access: off=%d\n",
			regno, tname, off);
		return -EACCES;
	}
	if (!tnum_is_const(reg->var_off) || reg->var_off.value) {
		char tn_buf[48];

		tnum_strn(tn_buf, sizeof(tn_buf), reg->var_off);
		verbose(env,
			"R%d is ptr_%s invalid variable offset: off=%d, var_off=%s\n",
			regno, tname, off, tn_buf);
		return -EACCES;
	}

	if (env->ops->btf_struct_access) {
		ret = env->ops->btf_struct_access(&env->log, reg->btf, t,
						  off, size, atype, &btf_id);
	} else {
		if (atype != BPF_READ) {
			verbose(env, "only read is supported\n");
			return -EACCES;
		}

		ret = btf_struct_access(&env->log, reg->btf, t, off, size,
					atype, &btf_id);
	}

	if (ret < 0)
		return ret;

	if (atype == BPF_READ && value_regno >= 0)
		mark_btf_ld_reg(env, regs, value_regno, ret, reg->btf, btf_id);

	return 0;
}