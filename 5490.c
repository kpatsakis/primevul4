static bool reg_scratched(const struct bpf_verifier_env *env, u32 regno)
{
	return (env->scratched_regs >> regno) & 1;
}