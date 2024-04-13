static bool verifier_state_scratched(const struct bpf_verifier_env *env)
{
	return env->scratched_regs || env->scratched_stack_slots;
}