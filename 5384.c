static bool states_equal(struct bpf_verifier_env *env,
			 struct bpf_verifier_state *old,
			 struct bpf_verifier_state *cur)
{
	int i;

	if (old->curframe != cur->curframe)
		return false;

	/* Verification state from speculative execution simulation
	 * must never prune a non-speculative execution one.
	 */
	if (old->speculative && !cur->speculative)
		return false;

	if (old->active_spin_lock != cur->active_spin_lock)
		return false;

	/* for states to be equal callsites have to be the same
	 * and all frame states need to be equivalent
	 */
	for (i = 0; i <= old->curframe; i++) {
		if (old->frame[i]->callsite != cur->frame[i]->callsite)
			return false;
		if (!func_states_equal(env, old->frame[i], cur->frame[i]))
			return false;
	}
	return true;
}