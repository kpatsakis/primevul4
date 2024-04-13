int check_ptr_off_reg(struct bpf_verifier_env *env,
		      const struct bpf_reg_state *reg, int regno)
{
	return __check_ptr_off_reg(env, reg, regno, false);
}