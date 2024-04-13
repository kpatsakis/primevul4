static void adjust_reg_min_max_vals(struct bpf_verifier_env *env,
				    struct bpf_insn *insn)
{
	struct bpf_reg_state *regs = env->cur_state.regs, *dst_reg;
	s64 min_val = BPF_REGISTER_MIN_RANGE;
	u64 max_val = BPF_REGISTER_MAX_RANGE;
	u8 opcode = BPF_OP(insn->code);

	dst_reg = &regs[insn->dst_reg];
	if (BPF_SRC(insn->code) == BPF_X) {
		check_reg_overflow(&regs[insn->src_reg]);
		min_val = regs[insn->src_reg].min_value;
		max_val = regs[insn->src_reg].max_value;

		/* If the source register is a random pointer then the
		 * min_value/max_value values represent the range of the known
		 * accesses into that value, not the actual min/max value of the
		 * register itself.  In this case we have to reset the reg range
		 * values so we know it is not safe to look at.
		 */
		if (regs[insn->src_reg].type != CONST_IMM &&
		    regs[insn->src_reg].type != UNKNOWN_VALUE) {
			min_val = BPF_REGISTER_MIN_RANGE;
			max_val = BPF_REGISTER_MAX_RANGE;
		}
	} else if (insn->imm < BPF_REGISTER_MAX_RANGE &&
		   (s64)insn->imm > BPF_REGISTER_MIN_RANGE) {
		min_val = max_val = insn->imm;
	}

	/* We don't know anything about what was done to this register, mark it
	 * as unknown.
	 */
	if (min_val == BPF_REGISTER_MIN_RANGE &&
	    max_val == BPF_REGISTER_MAX_RANGE) {
		reset_reg_range_values(regs, insn->dst_reg);
		return;
	}

	/* If one of our values was at the end of our ranges then we can't just
	 * do our normal operations to the register, we need to set the values
	 * to the min/max since they are undefined.
	 */
	if (min_val == BPF_REGISTER_MIN_RANGE)
		dst_reg->min_value = BPF_REGISTER_MIN_RANGE;
	if (max_val == BPF_REGISTER_MAX_RANGE)
		dst_reg->max_value = BPF_REGISTER_MAX_RANGE;

	switch (opcode) {
	case BPF_ADD:
		if (dst_reg->min_value != BPF_REGISTER_MIN_RANGE)
			dst_reg->min_value += min_val;
		if (dst_reg->max_value != BPF_REGISTER_MAX_RANGE)
			dst_reg->max_value += max_val;
		break;
	case BPF_SUB:
		if (dst_reg->min_value != BPF_REGISTER_MIN_RANGE)
			dst_reg->min_value -= min_val;
		if (dst_reg->max_value != BPF_REGISTER_MAX_RANGE)
			dst_reg->max_value -= max_val;
		break;
	case BPF_MUL:
		if (dst_reg->min_value != BPF_REGISTER_MIN_RANGE)
			dst_reg->min_value *= min_val;
		if (dst_reg->max_value != BPF_REGISTER_MAX_RANGE)
			dst_reg->max_value *= max_val;
		break;
	case BPF_AND:
		/* Disallow AND'ing of negative numbers, ain't nobody got time
		 * for that.  Otherwise the minimum is 0 and the max is the max
		 * value we could AND against.
		 */
		if (min_val < 0)
			dst_reg->min_value = BPF_REGISTER_MIN_RANGE;
		else
			dst_reg->min_value = 0;
		dst_reg->max_value = max_val;
		break;
	case BPF_LSH:
		/* Gotta have special overflow logic here, if we're shifting
		 * more than MAX_RANGE then just assume we have an invalid
		 * range.
		 */
		if (min_val > ilog2(BPF_REGISTER_MAX_RANGE))
			dst_reg->min_value = BPF_REGISTER_MIN_RANGE;
		else if (dst_reg->min_value != BPF_REGISTER_MIN_RANGE)
			dst_reg->min_value <<= min_val;

		if (max_val > ilog2(BPF_REGISTER_MAX_RANGE))
			dst_reg->max_value = BPF_REGISTER_MAX_RANGE;
		else if (dst_reg->max_value != BPF_REGISTER_MAX_RANGE)
			dst_reg->max_value <<= max_val;
		break;
	case BPF_RSH:
		/* RSH by a negative number is undefined, and the BPF_RSH is an
		 * unsigned shift, so make the appropriate casts.
		 */
		if (min_val < 0 || dst_reg->min_value < 0)
			dst_reg->min_value = BPF_REGISTER_MIN_RANGE;
		else
			dst_reg->min_value =
				(u64)(dst_reg->min_value) >> min_val;
		if (dst_reg->max_value != BPF_REGISTER_MAX_RANGE)
			dst_reg->max_value >>= max_val;
		break;
	default:
		reset_reg_range_values(regs, insn->dst_reg);
		break;
	}

	check_reg_overflow(dst_reg);
}
