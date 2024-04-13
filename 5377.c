static int __mark_chain_precision(struct bpf_verifier_env *env, int regno,
				  int spi)
{
	struct bpf_verifier_state *st = env->cur_state;
	int first_idx = st->first_insn_idx;
	int last_idx = env->insn_idx;
	struct bpf_func_state *func;
	struct bpf_reg_state *reg;
	u32 reg_mask = regno >= 0 ? 1u << regno : 0;
	u64 stack_mask = spi >= 0 ? 1ull << spi : 0;
	bool skip_first = true;
	bool new_marks = false;
	int i, err;

	if (!env->bpf_capable)
		return 0;

	func = st->frame[st->curframe];
	if (regno >= 0) {
		reg = &func->regs[regno];
		if (reg->type != SCALAR_VALUE) {
			WARN_ONCE(1, "backtracing misuse");
			return -EFAULT;
		}
		if (!reg->precise)
			new_marks = true;
		else
			reg_mask = 0;
		reg->precise = true;
	}

	while (spi >= 0) {
		if (!is_spilled_reg(&func->stack[spi])) {
			stack_mask = 0;
			break;
		}
		reg = &func->stack[spi].spilled_ptr;
		if (reg->type != SCALAR_VALUE) {
			stack_mask = 0;
			break;
		}
		if (!reg->precise)
			new_marks = true;
		else
			stack_mask = 0;
		reg->precise = true;
		break;
	}

	if (!new_marks)
		return 0;
	if (!reg_mask && !stack_mask)
		return 0;
	for (;;) {
		DECLARE_BITMAP(mask, 64);
		u32 history = st->jmp_history_cnt;

		if (env->log.level & BPF_LOG_LEVEL2)
			verbose(env, "last_idx %d first_idx %d\n", last_idx, first_idx);
		for (i = last_idx;;) {
			if (skip_first) {
				err = 0;
				skip_first = false;
			} else {
				err = backtrack_insn(env, i, &reg_mask, &stack_mask);
			}
			if (err == -ENOTSUPP) {
				mark_all_scalars_precise(env, st);
				return 0;
			} else if (err) {
				return err;
			}
			if (!reg_mask && !stack_mask)
				/* Found assignment(s) into tracked register in this state.
				 * Since this state is already marked, just return.
				 * Nothing to be tracked further in the parent state.
				 */
				return 0;
			if (i == first_idx)
				break;
			i = get_prev_insn_idx(st, i, &history);
			if (i >= env->prog->len) {
				/* This can happen if backtracking reached insn 0
				 * and there are still reg_mask or stack_mask
				 * to backtrack.
				 * It means the backtracking missed the spot where
				 * particular register was initialized with a constant.
				 */
				verbose(env, "BUG backtracking idx %d\n", i);
				WARN_ONCE(1, "verifier backtracking bug");
				return -EFAULT;
			}
		}
		st = st->parent;
		if (!st)
			break;

		new_marks = false;
		func = st->frame[st->curframe];
		bitmap_from_u64(mask, reg_mask);
		for_each_set_bit(i, mask, 32) {
			reg = &func->regs[i];
			if (reg->type != SCALAR_VALUE) {
				reg_mask &= ~(1u << i);
				continue;
			}
			if (!reg->precise)
				new_marks = true;
			reg->precise = true;
		}

		bitmap_from_u64(mask, stack_mask);
		for_each_set_bit(i, mask, 64) {
			if (i >= func->allocated_stack / BPF_REG_SIZE) {
				/* the sequence of instructions:
				 * 2: (bf) r3 = r10
				 * 3: (7b) *(u64 *)(r3 -8) = r0
				 * 4: (79) r4 = *(u64 *)(r10 -8)
				 * doesn't contain jmps. It's backtracked
				 * as a single block.
				 * During backtracking insn 3 is not recognized as
				 * stack access, so at the end of backtracking
				 * stack slot fp-8 is still marked in stack_mask.
				 * However the parent state may not have accessed
				 * fp-8 and it's "unallocated" stack space.
				 * In such case fallback to conservative.
				 */
				mark_all_scalars_precise(env, st);
				return 0;
			}

			if (!is_spilled_reg(&func->stack[i])) {
				stack_mask &= ~(1ull << i);
				continue;
			}
			reg = &func->stack[i].spilled_ptr;
			if (reg->type != SCALAR_VALUE) {
				stack_mask &= ~(1ull << i);
				continue;
			}
			if (!reg->precise)
				new_marks = true;
			reg->precise = true;
		}
		if (env->log.level & BPF_LOG_LEVEL2) {
			verbose(env, "parent %s regs=%x stack=%llx marks:",
				new_marks ? "didn't have" : "already had",
				reg_mask, stack_mask);
			print_verifier_state(env, func, true);
		}

		if (!reg_mask && !stack_mask)
			break;
		if (!new_marks)
			break;

		last_idx = st->last_insn_idx;
		first_idx = st->first_insn_idx;
	}
	return 0;
}