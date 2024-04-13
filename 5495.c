static bool stack_slot_scratched(const struct bpf_verifier_env *env, u64 regno)
{
	return (env->scratched_stack_slots >> regno) & 1;
}