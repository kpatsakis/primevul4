static void mark_verifier_state_clean(struct bpf_verifier_env *env)
{
	env->scratched_regs = 0U;
	env->scratched_stack_slots = 0ULL;
}