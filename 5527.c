int map_set_for_each_callback_args(struct bpf_verifier_env *env,
				   struct bpf_func_state *caller,
				   struct bpf_func_state *callee)
{
	/* bpf_for_each_map_elem(struct bpf_map *map, void *callback_fn,
	 *      void *callback_ctx, u64 flags);
	 * callback_fn(struct bpf_map *map, void *key, void *value,
	 *      void *callback_ctx);
	 */
	callee->regs[BPF_REG_1] = caller->regs[BPF_REG_1];

	callee->regs[BPF_REG_2].type = PTR_TO_MAP_KEY;
	__mark_reg_known_zero(&callee->regs[BPF_REG_2]);
	callee->regs[BPF_REG_2].map_ptr = caller->regs[BPF_REG_1].map_ptr;

	callee->regs[BPF_REG_3].type = PTR_TO_MAP_VALUE;
	__mark_reg_known_zero(&callee->regs[BPF_REG_3]);
	callee->regs[BPF_REG_3].map_ptr = caller->regs[BPF_REG_1].map_ptr;

	/* pointer to stack or null */
	callee->regs[BPF_REG_4] = caller->regs[BPF_REG_3];

	/* unused */
	__mark_reg_not_init(env, &callee->regs[BPF_REG_5]);
	return 0;
}