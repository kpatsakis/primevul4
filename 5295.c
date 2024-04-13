static int __check_func_call(struct bpf_verifier_env *env, struct bpf_insn *insn,
			     int *insn_idx, int subprog,
			     set_callee_state_fn set_callee_state_cb)
{
	struct bpf_verifier_state *state = env->cur_state;
	struct bpf_func_info_aux *func_info_aux;
	struct bpf_func_state *caller, *callee;
	int err;
	bool is_global = false;

	if (state->curframe + 1 >= MAX_CALL_FRAMES) {
		verbose(env, "the call stack of %d frames is too deep\n",
			state->curframe + 2);
		return -E2BIG;
	}

	caller = state->frame[state->curframe];
	if (state->frame[state->curframe + 1]) {
		verbose(env, "verifier bug. Frame %d already allocated\n",
			state->curframe + 1);
		return -EFAULT;
	}

	func_info_aux = env->prog->aux->func_info_aux;
	if (func_info_aux)
		is_global = func_info_aux[subprog].linkage == BTF_FUNC_GLOBAL;
	err = btf_check_subprog_arg_match(env, subprog, caller->regs);
	if (err == -EFAULT)
		return err;
	if (is_global) {
		if (err) {
			verbose(env, "Caller passes invalid args into func#%d\n",
				subprog);
			return err;
		} else {
			if (env->log.level & BPF_LOG_LEVEL)
				verbose(env,
					"Func#%d is global and valid. Skipping.\n",
					subprog);
			clear_caller_saved_regs(env, caller->regs);

			/* All global functions return a 64-bit SCALAR_VALUE */
			mark_reg_unknown(env, caller->regs, BPF_REG_0);
			caller->regs[BPF_REG_0].subreg_def = DEF_NOT_SUBREG;

			/* continue with next insn after call */
			return 0;
		}
	}

	if (insn->code == (BPF_JMP | BPF_CALL) &&
	    insn->src_reg == 0 &&
	    insn->imm == BPF_FUNC_timer_set_callback) {
		struct bpf_verifier_state *async_cb;

		/* there is no real recursion here. timer callbacks are async */
		env->subprog_info[subprog].is_async_cb = true;
		async_cb = push_async_cb(env, env->subprog_info[subprog].start,
					 *insn_idx, subprog);
		if (!async_cb)
			return -EFAULT;
		callee = async_cb->frame[0];
		callee->async_entry_cnt = caller->async_entry_cnt + 1;

		/* Convert bpf_timer_set_callback() args into timer callback args */
		err = set_callee_state_cb(env, caller, callee, *insn_idx);
		if (err)
			return err;

		clear_caller_saved_regs(env, caller->regs);
		mark_reg_unknown(env, caller->regs, BPF_REG_0);
		caller->regs[BPF_REG_0].subreg_def = DEF_NOT_SUBREG;
		/* continue with next insn after call */
		return 0;
	}

	callee = kzalloc(sizeof(*callee), GFP_KERNEL);
	if (!callee)
		return -ENOMEM;
	state->frame[state->curframe + 1] = callee;

	/* callee cannot access r0, r6 - r9 for reading and has to write
	 * into its own stack before reading from it.
	 * callee can read/write into caller's stack
	 */
	init_func_state(env, callee,
			/* remember the callsite, it will be used by bpf_exit */
			*insn_idx /* callsite */,
			state->curframe + 1 /* frameno within this callchain */,
			subprog /* subprog number within this prog */);

	/* Transfer references to the callee */
	err = copy_reference_state(callee, caller);
	if (err)
		return err;

	err = set_callee_state_cb(env, caller, callee, *insn_idx);
	if (err)
		return err;

	clear_caller_saved_regs(env, caller->regs);

	/* only increment it after check_reg_arg() finished */
	state->curframe++;

	/* and go analyze first insn of the callee */
	*insn_idx = env->subprog_info[subprog].start - 1;

	if (env->log.level & BPF_LOG_LEVEL) {
		verbose(env, "caller:\n");
		print_verifier_state(env, caller, true);
		verbose(env, "callee:\n");
		print_verifier_state(env, callee, true);
	}
	return 0;
}