static void find_good_pkt_pointers(struct bpf_verifier_state *state,
				   struct bpf_reg_state *dst_reg)
{
	struct bpf_reg_state *regs = state->regs, *reg;
	int i;

	/* LLVM can generate two kind of checks:
	 *
	 * Type 1:
	 *
	 *   r2 = r3;
	 *   r2 += 8;
	 *   if (r2 > pkt_end) goto <handle exception>
	 *   <access okay>
	 *
	 *   Where:
	 *     r2 == dst_reg, pkt_end == src_reg
	 *     r2=pkt(id=n,off=8,r=0)
	 *     r3=pkt(id=n,off=0,r=0)
	 *
	 * Type 2:
	 *
	 *   r2 = r3;
	 *   r2 += 8;
	 *   if (pkt_end >= r2) goto <access okay>
	 *   <handle exception>
	 *
	 *   Where:
	 *     pkt_end == dst_reg, r2 == src_reg
	 *     r2=pkt(id=n,off=8,r=0)
	 *     r3=pkt(id=n,off=0,r=0)
	 *
	 * Find register r3 and mark its range as r3=pkt(id=n,off=0,r=8)
	 * so that range of bytes [r3, r3 + 8) is safe to access.
	 */

	for (i = 0; i < MAX_BPF_REG; i++)
		if (regs[i].type == PTR_TO_PACKET && regs[i].id == dst_reg->id)
			/* keep the maximum range already checked */
			regs[i].range = max(regs[i].range, dst_reg->off);

	for (i = 0; i < MAX_BPF_STACK; i += BPF_REG_SIZE) {
		if (state->stack_slot_type[i] != STACK_SPILL)
			continue;
		reg = &state->spilled_regs[i / BPF_REG_SIZE];
		if (reg->type == PTR_TO_PACKET && reg->id == dst_reg->id)
			reg->range = max(reg->range, dst_reg->off);
	}
}
