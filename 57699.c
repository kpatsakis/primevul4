static int check_packet_ptr_add(struct bpf_verifier_env *env,
				struct bpf_insn *insn)
{
	struct bpf_reg_state *regs = env->cur_state.regs;
	struct bpf_reg_state *dst_reg = &regs[insn->dst_reg];
	struct bpf_reg_state *src_reg = &regs[insn->src_reg];
	struct bpf_reg_state tmp_reg;
	s32 imm;

	if (BPF_SRC(insn->code) == BPF_K) {
		/* pkt_ptr += imm */
		imm = insn->imm;

add_imm:
		if (imm < 0) {
			verbose("addition of negative constant to packet pointer is not allowed\n");
			return -EACCES;
		}
		if (imm >= MAX_PACKET_OFF ||
		    imm + dst_reg->off >= MAX_PACKET_OFF) {
			verbose("constant %d is too large to add to packet pointer\n",
				imm);
			return -EACCES;
		}
		/* a constant was added to pkt_ptr.
		 * Remember it while keeping the same 'id'
		 */
		dst_reg->off += imm;
	} else {
		if (src_reg->type == PTR_TO_PACKET) {
			/* R6=pkt(id=0,off=0,r=62) R7=imm22; r7 += r6 */
			tmp_reg = *dst_reg;  /* save r7 state */
			*dst_reg = *src_reg; /* copy pkt_ptr state r6 into r7 */
			src_reg = &tmp_reg;  /* pretend it's src_reg state */
			/* if the checks below reject it, the copy won't matter,
			 * since we're rejecting the whole program. If all ok,
			 * then imm22 state will be added to r7
			 * and r7 will be pkt(id=0,off=22,r=62) while
			 * r6 will stay as pkt(id=0,off=0,r=62)
			 */
		}

		if (src_reg->type == CONST_IMM) {
			/* pkt_ptr += reg where reg is known constant */
			imm = src_reg->imm;
			goto add_imm;
		}
		/* disallow pkt_ptr += reg
		 * if reg is not uknown_value with guaranteed zero upper bits
		 * otherwise pkt_ptr may overflow and addition will become
		 * subtraction which is not allowed
		 */
		if (src_reg->type != UNKNOWN_VALUE) {
			verbose("cannot add '%s' to ptr_to_packet\n",
				reg_type_str[src_reg->type]);
			return -EACCES;
		}
		if (src_reg->imm < 48) {
			verbose("cannot add integer value with %lld upper zero bits to ptr_to_packet\n",
				src_reg->imm);
			return -EACCES;
		}
		/* dst_reg stays as pkt_ptr type and since some positive
		 * integer value was added to the pointer, increment its 'id'
		 */
		dst_reg->id = ++env->id_gen;

		/* something was added to pkt_ptr, set range and off to zero */
		dst_reg->off = 0;
		dst_reg->range = 0;
	}
	return 0;
}
