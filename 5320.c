static void reg_set_min_max(struct bpf_reg_state *true_reg,
			    struct bpf_reg_state *false_reg,
			    u64 val, u32 val32,
			    u8 opcode, bool is_jmp32)
{
	struct tnum false_32off = tnum_subreg(false_reg->var_off);
	struct tnum false_64off = false_reg->var_off;
	struct tnum true_32off = tnum_subreg(true_reg->var_off);
	struct tnum true_64off = true_reg->var_off;
	s64 sval = (s64)val;
	s32 sval32 = (s32)val32;

	/* If the dst_reg is a pointer, we can't learn anything about its
	 * variable offset from the compare (unless src_reg were a pointer into
	 * the same object, but we don't bother with that.
	 * Since false_reg and true_reg have the same type by construction, we
	 * only need to check one of them for pointerness.
	 */
	if (__is_pointer_value(false, false_reg))
		return;

	switch (opcode) {
	case BPF_JEQ:
	case BPF_JNE:
	{
		struct bpf_reg_state *reg =
			opcode == BPF_JEQ ? true_reg : false_reg;

		/* JEQ/JNE comparison doesn't change the register equivalence.
		 * r1 = r2;
		 * if (r1 == 42) goto label;
		 * ...
		 * label: // here both r1 and r2 are known to be 42.
		 *
		 * Hence when marking register as known preserve it's ID.
		 */
		if (is_jmp32)
			__mark_reg32_known(reg, val32);
		else
			___mark_reg_known(reg, val);
		break;
	}
	case BPF_JSET:
		if (is_jmp32) {
			false_32off = tnum_and(false_32off, tnum_const(~val32));
			if (is_power_of_2(val32))
				true_32off = tnum_or(true_32off,
						     tnum_const(val32));
		} else {
			false_64off = tnum_and(false_64off, tnum_const(~val));
			if (is_power_of_2(val))
				true_64off = tnum_or(true_64off,
						     tnum_const(val));
		}
		break;
	case BPF_JGE:
	case BPF_JGT:
	{
		if (is_jmp32) {
			u32 false_umax = opcode == BPF_JGT ? val32  : val32 - 1;
			u32 true_umin = opcode == BPF_JGT ? val32 + 1 : val32;

			false_reg->u32_max_value = min(false_reg->u32_max_value,
						       false_umax);
			true_reg->u32_min_value = max(true_reg->u32_min_value,
						      true_umin);
		} else {
			u64 false_umax = opcode == BPF_JGT ? val    : val - 1;
			u64 true_umin = opcode == BPF_JGT ? val + 1 : val;

			false_reg->umax_value = min(false_reg->umax_value, false_umax);
			true_reg->umin_value = max(true_reg->umin_value, true_umin);
		}
		break;
	}
	case BPF_JSGE:
	case BPF_JSGT:
	{
		if (is_jmp32) {
			s32 false_smax = opcode == BPF_JSGT ? sval32    : sval32 - 1;
			s32 true_smin = opcode == BPF_JSGT ? sval32 + 1 : sval32;

			false_reg->s32_max_value = min(false_reg->s32_max_value, false_smax);
			true_reg->s32_min_value = max(true_reg->s32_min_value, true_smin);
		} else {
			s64 false_smax = opcode == BPF_JSGT ? sval    : sval - 1;
			s64 true_smin = opcode == BPF_JSGT ? sval + 1 : sval;

			false_reg->smax_value = min(false_reg->smax_value, false_smax);
			true_reg->smin_value = max(true_reg->smin_value, true_smin);
		}
		break;
	}
	case BPF_JLE:
	case BPF_JLT:
	{
		if (is_jmp32) {
			u32 false_umin = opcode == BPF_JLT ? val32  : val32 + 1;
			u32 true_umax = opcode == BPF_JLT ? val32 - 1 : val32;

			false_reg->u32_min_value = max(false_reg->u32_min_value,
						       false_umin);
			true_reg->u32_max_value = min(true_reg->u32_max_value,
						      true_umax);
		} else {
			u64 false_umin = opcode == BPF_JLT ? val    : val + 1;
			u64 true_umax = opcode == BPF_JLT ? val - 1 : val;

			false_reg->umin_value = max(false_reg->umin_value, false_umin);
			true_reg->umax_value = min(true_reg->umax_value, true_umax);
		}
		break;
	}
	case BPF_JSLE:
	case BPF_JSLT:
	{
		if (is_jmp32) {
			s32 false_smin = opcode == BPF_JSLT ? sval32    : sval32 + 1;
			s32 true_smax = opcode == BPF_JSLT ? sval32 - 1 : sval32;

			false_reg->s32_min_value = max(false_reg->s32_min_value, false_smin);
			true_reg->s32_max_value = min(true_reg->s32_max_value, true_smax);
		} else {
			s64 false_smin = opcode == BPF_JSLT ? sval    : sval + 1;
			s64 true_smax = opcode == BPF_JSLT ? sval - 1 : sval;

			false_reg->smin_value = max(false_reg->smin_value, false_smin);
			true_reg->smax_value = min(true_reg->smax_value, true_smax);
		}
		break;
	}
	default:
		return;
	}

	if (is_jmp32) {
		false_reg->var_off = tnum_or(tnum_clear_subreg(false_64off),
					     tnum_subreg(false_32off));
		true_reg->var_off = tnum_or(tnum_clear_subreg(true_64off),
					    tnum_subreg(true_32off));
		__reg_combine_32_into_64(false_reg);
		__reg_combine_32_into_64(true_reg);
	} else {
		false_reg->var_off = false_64off;
		true_reg->var_off = true_64off;
		__reg_combine_64_into_32(false_reg);
		__reg_combine_64_into_32(true_reg);
	}
}