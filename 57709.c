static int evaluate_reg_alu(struct bpf_verifier_env *env, struct bpf_insn *insn)
{
	struct bpf_reg_state *regs = env->cur_state.regs;
	struct bpf_reg_state *dst_reg = &regs[insn->dst_reg];
	u8 opcode = BPF_OP(insn->code);
	s64 imm_log2;

	/* for type == UNKNOWN_VALUE:
	 * imm > 0 -> number of zero upper bits
	 * imm == 0 -> don't track which is the same as all bits can be non-zero
	 */

	if (BPF_SRC(insn->code) == BPF_X) {
		struct bpf_reg_state *src_reg = &regs[insn->src_reg];

		if (src_reg->type == UNKNOWN_VALUE && src_reg->imm > 0 &&
		    dst_reg->imm && opcode == BPF_ADD) {
			/* dreg += sreg
			 * where both have zero upper bits. Adding them
			 * can only result making one more bit non-zero
			 * in the larger value.
			 * Ex. 0xffff (imm=48) + 1 (imm=63) = 0x10000 (imm=47)
			 *     0xffff (imm=48) + 0xffff = 0x1fffe (imm=47)
			 */
			dst_reg->imm = min(dst_reg->imm, src_reg->imm);
			dst_reg->imm--;
			return 0;
		}
		if (src_reg->type == CONST_IMM && src_reg->imm > 0 &&
		    dst_reg->imm && opcode == BPF_ADD) {
			/* dreg += sreg
			 * where dreg has zero upper bits and sreg is const.
			 * Adding them can only result making one more bit
			 * non-zero in the larger value.
			 */
			imm_log2 = __ilog2_u64((long long)src_reg->imm);
			dst_reg->imm = min(dst_reg->imm, 63 - imm_log2);
			dst_reg->imm--;
			return 0;
		}
		/* all other cases non supported yet, just mark dst_reg */
		dst_reg->imm = 0;
		return 0;
	}

	/* sign extend 32-bit imm into 64-bit to make sure that
	 * negative values occupy bit 63. Note ilog2() would have
	 * been incorrect, since sizeof(insn->imm) == 4
	 */
	imm_log2 = __ilog2_u64((long long)insn->imm);

	if (dst_reg->imm && opcode == BPF_LSH) {
		/* reg <<= imm
		 * if reg was a result of 2 byte load, then its imm == 48
		 * which means that upper 48 bits are zero and shifting this reg
		 * left by 4 would mean that upper 44 bits are still zero
		 */
		dst_reg->imm -= insn->imm;
	} else if (dst_reg->imm && opcode == BPF_MUL) {
		/* reg *= imm
		 * if multiplying by 14 subtract 4
		 * This is conservative calculation of upper zero bits.
		 * It's not trying to special case insn->imm == 1 or 0 cases
		 */
		dst_reg->imm -= imm_log2 + 1;
	} else if (opcode == BPF_AND) {
		/* reg &= imm */
		dst_reg->imm = 63 - imm_log2;
	} else if (dst_reg->imm && opcode == BPF_ADD) {
		/* reg += imm */
		dst_reg->imm = min(dst_reg->imm, 63 - imm_log2);
		dst_reg->imm--;
	} else if (opcode == BPF_RSH) {
		/* reg >>= imm
		 * which means that after right shift, upper bits will be zero
		 * note that verifier already checked that
		 * 0 <= imm < 64 for shift insn
		 */
		dst_reg->imm += insn->imm;
		if (unlikely(dst_reg->imm > 64))
			/* some dumb code did:
			 * r2 = *(u32 *)mem;
			 * r2 >>= 32;
			 * and all bits are zero now */
			dst_reg->imm = 64;
	} else {
		/* all other alu ops, means that we don't know what will
		 * happen to the value, mark it with unknown number of zero bits
		 */
		dst_reg->imm = 0;
	}

	if (dst_reg->imm < 0) {
		/* all 64 bits of the register can contain non-zero bits
		 * and such value cannot be added to ptr_to_packet, since it
		 * may overflow, mark it as unknown to avoid further eval
		 */
		dst_reg->imm = 0;
	}
	return 0;
}
