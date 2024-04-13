static bool allow_tail_call_in_subprogs(struct bpf_verifier_env *env)
{
	return env->prog->jit_requested && IS_ENABLED(CONFIG_X86_64);
}