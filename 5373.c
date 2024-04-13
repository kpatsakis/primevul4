static void mark_reg_stack_read(struct bpf_verifier_env *env,
				/* func where src register points to */
				struct bpf_func_state *ptr_state,
				int min_off, int max_off, int dst_regno)
{
	struct bpf_verifier_state *vstate = env->cur_state;
	struct bpf_func_state *state = vstate->frame[vstate->curframe];
	int i, slot, spi;
	u8 *stype;
	int zeros = 0;

	for (i = min_off; i < max_off; i++) {
		slot = -i - 1;
		spi = slot / BPF_REG_SIZE;
		stype = ptr_state->stack[spi].slot_type;
		if (stype[slot % BPF_REG_SIZE] != STACK_ZERO)
			break;
		zeros++;
	}
	if (zeros == max_off - min_off) {
		/* any access_size read into register is zero extended,
		 * so the whole register == const_zero
		 */
		__mark_reg_const_zero(&state->regs[dst_regno]);
		/* backtracking doesn't support STACK_ZERO yet,
		 * so mark it precise here, so that later
		 * backtracking can stop here.
		 * Backtracking may not need this if this register
		 * doesn't participate in pointer adjustment.
		 * Forward propagation of precise flag is not
		 * necessary either. This mark is only to stop
		 * backtracking. Any register that contributed
		 * to const 0 was marked precise before spill.
		 */
		state->regs[dst_regno].precise = true;
	} else {
		/* have read misc data from the stack */
		mark_reg_unknown(env, state->regs, dst_regno);
	}
	state->regs[dst_regno].live |= REG_LIVE_WRITTEN;
}