static struct bpf_prog *bpf_patch_insn_data(struct bpf_verifier_env *env, u32 off,
					    const struct bpf_insn *patch, u32 len)
{
	struct bpf_prog *new_prog;
	struct bpf_insn_aux_data *new_data = NULL;

	if (len > 1) {
		new_data = vzalloc(array_size(env->prog->len + len - 1,
					      sizeof(struct bpf_insn_aux_data)));
		if (!new_data)
			return NULL;
	}

	new_prog = bpf_patch_insn_single(env->prog, off, patch, len);
	if (IS_ERR(new_prog)) {
		if (PTR_ERR(new_prog) == -ERANGE)
			verbose(env,
				"insn %d cannot be patched due to 16-bit range\n",
				env->insn_aux_data[off].orig_idx);
		vfree(new_data);
		return NULL;
	}
	adjust_insn_aux_data(env, new_data, new_prog, off, len);
	adjust_subprog_starts(env, off, len);
	adjust_poke_descs(new_prog, off, len);
	return new_prog;
}