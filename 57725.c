static int pop_stack(struct bpf_verifier_env *env, int *prev_insn_idx)
{
	struct bpf_verifier_stack_elem *elem;
	int insn_idx;

	if (env->head == NULL)
		return -1;

	memcpy(&env->cur_state, &env->head->st, sizeof(env->cur_state));
	insn_idx = env->head->insn_idx;
	if (prev_insn_idx)
		*prev_insn_idx = env->head->prev_insn_idx;
	elem = env->head->next;
	kfree(env->head);
	env->head = elem;
	env->stack_size--;
	return insn_idx;
}
