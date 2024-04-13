static void mark_reg_scratched(struct bpf_verifier_env *env, u32 regno)
{
	env->scratched_regs |= 1U << regno;
}