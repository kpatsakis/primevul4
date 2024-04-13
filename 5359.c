static int sanitize_err(struct bpf_verifier_env *env,
			const struct bpf_insn *insn, int reason,
			const struct bpf_reg_state *off_reg,
			const struct bpf_reg_state *dst_reg)
{
	static const char *err = "pointer arithmetic with it prohibited for !root";
	const char *op = BPF_OP(insn->code) == BPF_ADD ? "add" : "sub";
	u32 dst = insn->dst_reg, src = insn->src_reg;

	switch (reason) {
	case REASON_BOUNDS:
		verbose(env, "R%d has unknown scalar with mixed signed bounds, %s\n",
			off_reg == dst_reg ? dst : src, err);
		break;
	case REASON_TYPE:
		verbose(env, "R%d has pointer with unsupported alu operation, %s\n",
			off_reg == dst_reg ? src : dst, err);
		break;
	case REASON_PATHS:
		verbose(env, "R%d tried to %s from different maps, paths or scalars, %s\n",
			dst, op, err);
		break;
	case REASON_LIMIT:
		verbose(env, "R%d tried to %s beyond pointer bounds, %s\n",
			dst, op, err);
		break;
	case REASON_STACK:
		verbose(env, "R%d could not be pushed for speculative verification, %s\n",
			dst, err);
		break;
	default:
		verbose(env, "verifier internal error: unknown reason (%d)\n",
			reason);
		break;
	}

	return -EACCES;
}