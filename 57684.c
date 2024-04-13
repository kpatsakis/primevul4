static int check_alu_op(struct bpf_verifier_env *env, struct bpf_insn *insn)
{
	struct bpf_reg_state *regs = env->cur_state.regs, *dst_reg;
	u8 opcode = BPF_OP(insn->code);
	int err;

	if (opcode == BPF_END || opcode == BPF_NEG) {
		if (opcode == BPF_NEG) {
			if (BPF_SRC(insn->code) != 0 ||
			    insn->src_reg != BPF_REG_0 ||
			    insn->off != 0 || insn->imm != 0) {
				verbose("BPF_NEG uses reserved fields\n");
				return -EINVAL;
			}
		} else {
			if (insn->src_reg != BPF_REG_0 || insn->off != 0 ||
			    (insn->imm != 16 && insn->imm != 32 && insn->imm != 64)) {
				verbose("BPF_END uses reserved fields\n");
				return -EINVAL;
			}
		}

		/* check src operand */
		err = check_reg_arg(regs, insn->dst_reg, SRC_OP);
		if (err)
			return err;

		if (is_pointer_value(env, insn->dst_reg)) {
			verbose("R%d pointer arithmetic prohibited\n",
				insn->dst_reg);
			return -EACCES;
		}

		/* check dest operand */
		err = check_reg_arg(regs, insn->dst_reg, DST_OP);
		if (err)
			return err;

	} else if (opcode == BPF_MOV) {

		if (BPF_SRC(insn->code) == BPF_X) {
			if (insn->imm != 0 || insn->off != 0) {
				verbose("BPF_MOV uses reserved fields\n");
				return -EINVAL;
			}

			/* check src operand */
			err = check_reg_arg(regs, insn->src_reg, SRC_OP);
			if (err)
				return err;
		} else {
			if (insn->src_reg != BPF_REG_0 || insn->off != 0) {
				verbose("BPF_MOV uses reserved fields\n");
				return -EINVAL;
			}
		}

		/* check dest operand */
		err = check_reg_arg(regs, insn->dst_reg, DST_OP);
		if (err)
			return err;

		/* we are setting our register to something new, we need to
		 * reset its range values.
		 */
		reset_reg_range_values(regs, insn->dst_reg);

		if (BPF_SRC(insn->code) == BPF_X) {
			if (BPF_CLASS(insn->code) == BPF_ALU64) {
				/* case: R1 = R2
				 * copy register state to dest reg
				 */
				regs[insn->dst_reg] = regs[insn->src_reg];
			} else {
				if (is_pointer_value(env, insn->src_reg)) {
					verbose("R%d partial copy of pointer\n",
						insn->src_reg);
					return -EACCES;
				}
				mark_reg_unknown_value(regs, insn->dst_reg);
			}
		} else {
			/* case: R = imm
			 * remember the value we stored into this reg
			 */
			regs[insn->dst_reg].type = CONST_IMM;
			regs[insn->dst_reg].imm = insn->imm;
			regs[insn->dst_reg].max_value = insn->imm;
			regs[insn->dst_reg].min_value = insn->imm;
		}

	} else if (opcode > BPF_END) {
		verbose("invalid BPF_ALU opcode %x\n", opcode);
		return -EINVAL;

	} else {	/* all other ALU ops: and, sub, xor, add, ... */

		if (BPF_SRC(insn->code) == BPF_X) {
			if (insn->imm != 0 || insn->off != 0) {
				verbose("BPF_ALU uses reserved fields\n");
				return -EINVAL;
			}
			/* check src1 operand */
			err = check_reg_arg(regs, insn->src_reg, SRC_OP);
			if (err)
				return err;
		} else {
			if (insn->src_reg != BPF_REG_0 || insn->off != 0) {
				verbose("BPF_ALU uses reserved fields\n");
				return -EINVAL;
			}
		}

		/* check src2 operand */
		err = check_reg_arg(regs, insn->dst_reg, SRC_OP);
		if (err)
			return err;

		if ((opcode == BPF_MOD || opcode == BPF_DIV) &&
		    BPF_SRC(insn->code) == BPF_K && insn->imm == 0) {
			verbose("div by zero\n");
			return -EINVAL;
		}

		if ((opcode == BPF_LSH || opcode == BPF_RSH ||
		     opcode == BPF_ARSH) && BPF_SRC(insn->code) == BPF_K) {
			int size = BPF_CLASS(insn->code) == BPF_ALU64 ? 64 : 32;

			if (insn->imm < 0 || insn->imm >= size) {
				verbose("invalid shift %d\n", insn->imm);
				return -EINVAL;
			}
		}

		/* check dest operand */
		err = check_reg_arg(regs, insn->dst_reg, DST_OP_NO_MARK);
		if (err)
			return err;

		dst_reg = &regs[insn->dst_reg];

		/* first we want to adjust our ranges. */
		adjust_reg_min_max_vals(env, insn);

		/* pattern match 'bpf_add Rx, imm' instruction */
		if (opcode == BPF_ADD && BPF_CLASS(insn->code) == BPF_ALU64 &&
		    dst_reg->type == FRAME_PTR && BPF_SRC(insn->code) == BPF_K) {
			dst_reg->type = PTR_TO_STACK;
			dst_reg->imm = insn->imm;
			return 0;
		} else if (opcode == BPF_ADD &&
			   BPF_CLASS(insn->code) == BPF_ALU64 &&
			   dst_reg->type == PTR_TO_STACK &&
			   ((BPF_SRC(insn->code) == BPF_X &&
			     regs[insn->src_reg].type == CONST_IMM) ||
			    BPF_SRC(insn->code) == BPF_K)) {
			if (BPF_SRC(insn->code) == BPF_X)
				dst_reg->imm += regs[insn->src_reg].imm;
			else
				dst_reg->imm += insn->imm;
			return 0;
		} else if (opcode == BPF_ADD &&
			   BPF_CLASS(insn->code) == BPF_ALU64 &&
			   (dst_reg->type == PTR_TO_PACKET ||
			    (BPF_SRC(insn->code) == BPF_X &&
			     regs[insn->src_reg].type == PTR_TO_PACKET))) {
			/* ptr_to_packet += K|X */
			return check_packet_ptr_add(env, insn);
		} else if (BPF_CLASS(insn->code) == BPF_ALU64 &&
			   dst_reg->type == UNKNOWN_VALUE &&
			   env->allow_ptr_leaks) {
			/* unknown += K|X */
			return evaluate_reg_alu(env, insn);
		} else if (BPF_CLASS(insn->code) == BPF_ALU64 &&
			   dst_reg->type == CONST_IMM &&
			   env->allow_ptr_leaks) {
			/* reg_imm += K|X */
			return evaluate_reg_imm_alu(env, insn);
		} else if (is_pointer_value(env, insn->dst_reg)) {
			verbose("R%d pointer arithmetic prohibited\n",
				insn->dst_reg);
			return -EACCES;
		} else if (BPF_SRC(insn->code) == BPF_X &&
			   is_pointer_value(env, insn->src_reg)) {
			verbose("R%d pointer arithmetic prohibited\n",
				insn->src_reg);
			return -EACCES;
		}

		/* If we did pointer math on a map value then just set it to our
		 * PTR_TO_MAP_VALUE_ADJ type so we can deal with any stores or
		 * loads to this register appropriately, otherwise just mark the
		 * register as unknown.
		 */
		if (env->allow_ptr_leaks &&
		    BPF_CLASS(insn->code) == BPF_ALU64 && opcode == BPF_ADD &&
		    (dst_reg->type == PTR_TO_MAP_VALUE ||
		     dst_reg->type == PTR_TO_MAP_VALUE_ADJ))
			dst_reg->type = PTR_TO_MAP_VALUE_ADJ;
		else
			mark_reg_unknown_value(regs, insn->dst_reg);
	}

	return 0;
}
