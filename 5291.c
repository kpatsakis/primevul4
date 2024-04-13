static void mark_btf_ld_reg(struct bpf_verifier_env *env,
			    struct bpf_reg_state *regs, u32 regno,
			    enum bpf_reg_type reg_type,
			    struct btf *btf, u32 btf_id)
{
	if (reg_type == SCALAR_VALUE) {
		mark_reg_unknown(env, regs, regno);
		return;
	}
	mark_reg_known_zero(env, regs, regno);
	regs[regno].type = PTR_TO_BTF_ID;
	regs[regno].btf = btf;
	regs[regno].btf_id = btf_id;
}