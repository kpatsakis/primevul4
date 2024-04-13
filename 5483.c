static int fixup_kfunc_call(struct bpf_verifier_env *env,
			    struct bpf_insn *insn)
{
	const struct bpf_kfunc_desc *desc;

	if (!insn->imm) {
		verbose(env, "invalid kernel function call not eliminated in verifier pass\n");
		return -EINVAL;
	}

	/* insn->imm has the btf func_id. Replace it with
	 * an address (relative to __bpf_base_call).
	 */
	desc = find_kfunc_desc(env->prog, insn->imm, insn->off);
	if (!desc) {
		verbose(env, "verifier internal error: kernel function descriptor not found for func_id %u\n",
			insn->imm);
		return -EFAULT;
	}

	insn->imm = desc->imm;

	return 0;
}