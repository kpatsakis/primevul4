static int is_state_visited(struct bpf_verifier_env *env, int insn_idx)
{
	struct bpf_verifier_state_list *new_sl;
	struct bpf_verifier_state_list *sl;

	sl = env->explored_states[insn_idx];
	if (!sl)
		/* this 'insn_idx' instruction wasn't marked, so we will not
		 * be doing state search here
		 */
		return 0;

	while (sl != STATE_LIST_MARK) {
		if (states_equal(env, &sl->state, &env->cur_state))
			/* reached equivalent register/stack state,
			 * prune the search
			 */
			return 1;
		sl = sl->next;
	}

	/* there were no equivalent states, remember current one.
	 * technically the current state is not proven to be safe yet,
	 * but it will either reach bpf_exit (which means it's safe) or
	 * it will be rejected. Since there are no loops, we won't be
	 * seeing this 'insn_idx' instruction again on the way to bpf_exit
	 */
	new_sl = kmalloc(sizeof(struct bpf_verifier_state_list), GFP_USER);
	if (!new_sl)
		return -ENOMEM;

	/* add new state to the head of linked list */
	memcpy(&new_sl->state, &env->cur_state, sizeof(env->cur_state));
	new_sl->next = env->explored_states[insn_idx];
	env->explored_states[insn_idx] = new_sl;
	return 0;
}
