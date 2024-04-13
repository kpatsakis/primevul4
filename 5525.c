static int set_loop_callback_state(struct bpf_verifier_env *env,
				   struct bpf_func_state *caller,
				   struct bpf_func_state *callee,
				   int insn_idx)
{
	/* bpf_loop(u32 nr_loops, void *callback_fn, void *callback_ctx,
	 *	    u64 flags);
	 * callback_fn(u32 index, void *callback_ctx);
	 */
	callee->regs[BPF_REG_1].type = SCALAR_VALUE;
	callee->regs[BPF_REG_2] = caller->regs[BPF_REG_3];

	/* unused */
	__mark_reg_not_init(env, &callee->regs[BPF_REG_3]);
	__mark_reg_not_init(env, &callee->regs[BPF_REG_4]);
	__mark_reg_not_init(env, &callee->regs[BPF_REG_5]);

	callee->in_callback_fn = true;
	return 0;
}