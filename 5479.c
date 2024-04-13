static int check_stack_read_fixed_off(struct bpf_verifier_env *env,
				      /* func where src register points to */
				      struct bpf_func_state *reg_state,
				      int off, int size, int dst_regno)
{
	struct bpf_verifier_state *vstate = env->cur_state;
	struct bpf_func_state *state = vstate->frame[vstate->curframe];
	int i, slot = -off - 1, spi = slot / BPF_REG_SIZE;
	struct bpf_reg_state *reg;
	u8 *stype, type;

	stype = reg_state->stack[spi].slot_type;
	reg = &reg_state->stack[spi].spilled_ptr;

	if (is_spilled_reg(&reg_state->stack[spi])) {
		u8 spill_size = 1;

		for (i = BPF_REG_SIZE - 1; i > 0 && stype[i - 1] == STACK_SPILL; i--)
			spill_size++;

		if (size != BPF_REG_SIZE || spill_size != BPF_REG_SIZE) {
			if (reg->type != SCALAR_VALUE) {
				verbose_linfo(env, env->insn_idx, "; ");
				verbose(env, "invalid size of register fill\n");
				return -EACCES;
			}

			mark_reg_read(env, reg, reg->parent, REG_LIVE_READ64);
			if (dst_regno < 0)
				return 0;

			if (!(off % BPF_REG_SIZE) && size == spill_size) {
				/* The earlier check_reg_arg() has decided the
				 * subreg_def for this insn.  Save it first.
				 */
				s32 subreg_def = state->regs[dst_regno].subreg_def;

				state->regs[dst_regno] = *reg;
				state->regs[dst_regno].subreg_def = subreg_def;
			} else {
				for (i = 0; i < size; i++) {
					type = stype[(slot - i) % BPF_REG_SIZE];
					if (type == STACK_SPILL)
						continue;
					if (type == STACK_MISC)
						continue;
					verbose(env, "invalid read from stack off %d+%d size %d\n",
						off, i, size);
					return -EACCES;
				}
				mark_reg_unknown(env, state->regs, dst_regno);
			}
			state->regs[dst_regno].live |= REG_LIVE_WRITTEN;
			return 0;
		}

		if (dst_regno >= 0) {
			/* restore register state from stack */
			state->regs[dst_regno] = *reg;
			/* mark reg as written since spilled pointer state likely
			 * has its liveness marks cleared by is_state_visited()
			 * which resets stack/reg liveness for state transitions
			 */
			state->regs[dst_regno].live |= REG_LIVE_WRITTEN;
		} else if (__is_pointer_value(env->allow_ptr_leaks, reg)) {
			/* If dst_regno==-1, the caller is asking us whether
			 * it is acceptable to use this value as a SCALAR_VALUE
			 * (e.g. for XADD).
			 * We must not allow unprivileged callers to do that
			 * with spilled pointers.
			 */
			verbose(env, "leaking pointer from stack off %d\n",
				off);
			return -EACCES;
		}
		mark_reg_read(env, reg, reg->parent, REG_LIVE_READ64);
	} else {
		for (i = 0; i < size; i++) {
			type = stype[(slot - i) % BPF_REG_SIZE];
			if (type == STACK_MISC)
				continue;
			if (type == STACK_ZERO)
				continue;
			verbose(env, "invalid read from stack off %d+%d size %d\n",
				off, i, size);
			return -EACCES;
		}
		mark_reg_read(env, reg, reg->parent, REG_LIVE_READ64);
		if (dst_regno >= 0)
			mark_reg_stack_read(env, reg_state, off, off + size, dst_regno);
	}
	return 0;
}