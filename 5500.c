static int set_timer_callback_state(struct bpf_verifier_env *env,
				    struct bpf_func_state *caller,
				    struct bpf_func_state *callee,
				    int insn_idx)
{
	struct bpf_map *map_ptr = caller->regs[BPF_REG_1].map_ptr;

	/* bpf_timer_set_callback(struct bpf_timer *timer, void *callback_fn);
	 * callback_fn(struct bpf_map *map, void *key, void *value);
	 */
	callee->regs[BPF_REG_1].type = CONST_PTR_TO_MAP;
	__mark_reg_known_zero(&callee->regs[BPF_REG_1]);
	callee->regs[BPF_REG_1].map_ptr = map_ptr;

	callee->regs[BPF_REG_2].type = PTR_TO_MAP_KEY;
	__mark_reg_known_zero(&callee->regs[BPF_REG_2]);
	callee->regs[BPF_REG_2].map_ptr = map_ptr;

	callee->regs[BPF_REG_3].type = PTR_TO_MAP_VALUE;
	__mark_reg_known_zero(&callee->regs[BPF_REG_3]);
	callee->regs[BPF_REG_3].map_ptr = map_ptr;

	/* unused */
	__mark_reg_not_init(env, &callee->regs[BPF_REG_4]);
	__mark_reg_not_init(env, &callee->regs[BPF_REG_5]);
	callee->in_async_callback_fn = true;
	return 0;
}