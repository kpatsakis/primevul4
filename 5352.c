static void mark_btf_func_reg_size(struct bpf_verifier_env *env, u32 regno,
				   size_t reg_size)
{
	struct bpf_reg_state *reg = &cur_regs(env)[regno];

	if (regno == BPF_REG_0) {
		/* Function return value */
		reg->live |= REG_LIVE_WRITTEN;
		reg->subreg_def = reg_size == sizeof(u64) ?
			DEF_NOT_SUBREG : env->insn_idx + 1;
	} else {
		/* Function argument */
		if (reg_size == sizeof(u64)) {
			mark_insn_zext(env, reg);
			mark_reg_read(env, reg, reg->parent, REG_LIVE_READ64);
		} else {
			mark_reg_read(env, reg, reg->parent, REG_LIVE_READ32);
		}
	}
}