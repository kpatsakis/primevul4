static int propagate_precision(struct bpf_verifier_env *env,
			       const struct bpf_verifier_state *old)
{
	struct bpf_reg_state *state_reg;
	struct bpf_func_state *state;
	int i, err = 0;

	state = old->frame[old->curframe];
	state_reg = state->regs;
	for (i = 0; i < BPF_REG_FP; i++, state_reg++) {
		if (state_reg->type != SCALAR_VALUE ||
		    !state_reg->precise)
			continue;
		if (env->log.level & BPF_LOG_LEVEL2)
			verbose(env, "propagating r%d\n", i);
		err = mark_chain_precision(env, i);
		if (err < 0)
			return err;
	}

	for (i = 0; i < state->allocated_stack / BPF_REG_SIZE; i++) {
		if (!is_spilled_reg(&state->stack[i]))
			continue;
		state_reg = &state->stack[i].spilled_ptr;
		if (state_reg->type != SCALAR_VALUE ||
		    !state_reg->precise)
			continue;
		if (env->log.level & BPF_LOG_LEVEL2)
			verbose(env, "propagating fp%d\n",
				(-i - 1) * BPF_REG_SIZE);
		err = mark_chain_precision_stack(env, i);
		if (err < 0)
			return err;
	}
	return 0;
}