static int set_find_vma_callback_state(struct bpf_verifier_env *env,
				       struct bpf_func_state *caller,
				       struct bpf_func_state *callee,
				       int insn_idx)
{
	/* bpf_find_vma(struct task_struct *task, u64 addr,
	 *               void *callback_fn, void *callback_ctx, u64 flags)
	 * (callback_fn)(struct task_struct *task,
	 *               struct vm_area_struct *vma, void *callback_ctx);
	 */
	callee->regs[BPF_REG_1] = caller->regs[BPF_REG_1];

	callee->regs[BPF_REG_2].type = PTR_TO_BTF_ID;
	__mark_reg_known_zero(&callee->regs[BPF_REG_2]);
	callee->regs[BPF_REG_2].btf =  btf_vmlinux;
	callee->regs[BPF_REG_2].btf_id = btf_tracing_ids[BTF_TRACING_TYPE_VMA],

	/* pointer to stack or null */
	callee->regs[BPF_REG_3] = caller->regs[BPF_REG_4];

	/* unused */
	__mark_reg_not_init(env, &callee->regs[BPF_REG_4]);
	__mark_reg_not_init(env, &callee->regs[BPF_REG_5]);
	callee->in_callback_fn = true;
	return 0;
}