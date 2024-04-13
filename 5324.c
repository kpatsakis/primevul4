static void verbose_invalid_scalar(struct bpf_verifier_env *env,
				   struct bpf_reg_state *reg,
				   struct tnum *range, const char *ctx,
				   const char *reg_name)
{
	char tn_buf[48];

	verbose(env, "At %s the register %s ", ctx, reg_name);
	if (!tnum_is_unknown(reg->var_off)) {
		tnum_strn(tn_buf, sizeof(tn_buf), reg->var_off);
		verbose(env, "has value %s", tn_buf);
	} else {
		verbose(env, "has unknown scalar value");
	}
	tnum_strn(tn_buf, sizeof(tn_buf), *range);
	verbose(env, " should have been in %s\n", tn_buf);
}