static bool is_pointer_value(struct bpf_verifier_env *env, int regno)
{
	if (env->allow_ptr_leaks)
		return false;

	switch (env->cur_state.regs[regno].type) {
	case UNKNOWN_VALUE:
	case CONST_IMM:
		return false;
	default:
		return true;
	}
}
