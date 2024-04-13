static int is_branch32_taken(struct bpf_reg_state *reg, u32 val, u8 opcode)
{
	struct tnum subreg = tnum_subreg(reg->var_off);
	s32 sval = (s32)val;

	switch (opcode) {
	case BPF_JEQ:
		if (tnum_is_const(subreg))
			return !!tnum_equals_const(subreg, val);
		break;
	case BPF_JNE:
		if (tnum_is_const(subreg))
			return !tnum_equals_const(subreg, val);
		break;
	case BPF_JSET:
		if ((~subreg.mask & subreg.value) & val)
			return 1;
		if (!((subreg.mask | subreg.value) & val))
			return 0;
		break;
	case BPF_JGT:
		if (reg->u32_min_value > val)
			return 1;
		else if (reg->u32_max_value <= val)
			return 0;
		break;
	case BPF_JSGT:
		if (reg->s32_min_value > sval)
			return 1;
		else if (reg->s32_max_value <= sval)
			return 0;
		break;
	case BPF_JLT:
		if (reg->u32_max_value < val)
			return 1;
		else if (reg->u32_min_value >= val)
			return 0;
		break;
	case BPF_JSLT:
		if (reg->s32_max_value < sval)
			return 1;
		else if (reg->s32_min_value >= sval)
			return 0;
		break;
	case BPF_JGE:
		if (reg->u32_min_value >= val)
			return 1;
		else if (reg->u32_max_value < val)
			return 0;
		break;
	case BPF_JSGE:
		if (reg->s32_min_value >= sval)
			return 1;
		else if (reg->s32_max_value < sval)
			return 0;
		break;
	case BPF_JLE:
		if (reg->u32_max_value <= val)
			return 1;
		else if (reg->u32_min_value > val)
			return 0;
		break;
	case BPF_JSLE:
		if (reg->s32_max_value <= sval)
			return 1;
		else if (reg->s32_min_value > sval)
			return 0;
		break;
	}

	return -1;
}