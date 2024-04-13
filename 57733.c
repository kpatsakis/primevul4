static bool states_equal(struct bpf_verifier_env *env,
			 struct bpf_verifier_state *old,
			 struct bpf_verifier_state *cur)
{
	bool varlen_map_access = env->varlen_map_value_access;
	struct bpf_reg_state *rold, *rcur;
	int i;

	for (i = 0; i < MAX_BPF_REG; i++) {
		rold = &old->regs[i];
		rcur = &cur->regs[i];

		if (memcmp(rold, rcur, sizeof(*rold)) == 0)
			continue;

		/* If the ranges were not the same, but everything else was and
		 * we didn't do a variable access into a map then we are a-ok.
		 */
		if (!varlen_map_access &&
		    memcmp(rold, rcur, offsetofend(struct bpf_reg_state, id)) == 0)
			continue;

		/* If we didn't map access then again we don't care about the
		 * mismatched range values and it's ok if our old type was
		 * UNKNOWN and we didn't go to a NOT_INIT'ed reg.
		 */
		if (rold->type == NOT_INIT ||
		    (!varlen_map_access && rold->type == UNKNOWN_VALUE &&
		     rcur->type != NOT_INIT))
			continue;

		if (rold->type == PTR_TO_PACKET && rcur->type == PTR_TO_PACKET &&
		    compare_ptrs_to_packet(rold, rcur))
			continue;

		return false;
	}

	for (i = 0; i < MAX_BPF_STACK; i++) {
		if (old->stack_slot_type[i] == STACK_INVALID)
			continue;
		if (old->stack_slot_type[i] != cur->stack_slot_type[i])
			/* Ex: old explored (safe) state has STACK_SPILL in
			 * this stack slot, but current has has STACK_MISC ->
			 * this verifier states are not equivalent,
			 * return false to continue verification of this path
			 */
			return false;
		if (i % BPF_REG_SIZE)
			continue;
		if (memcmp(&old->spilled_regs[i / BPF_REG_SIZE],
			   &cur->spilled_regs[i / BPF_REG_SIZE],
			   sizeof(old->spilled_regs[0])))
			/* when explored and current stack slot types are
			 * the same, check that stored pointers types
			 * are the same as well.
			 * Ex: explored safe path could have stored
			 * (bpf_reg_state) {.type = PTR_TO_STACK, .imm = -8}
			 * but current path has stored:
			 * (bpf_reg_state) {.type = PTR_TO_STACK, .imm = -16}
			 * such verifier states are not equivalent.
			 * return false to continue verification of this path
			 */
			return false;
		else
			continue;
	}
	return true;
}
