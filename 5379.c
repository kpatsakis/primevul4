static void print_insn_state(struct bpf_verifier_env *env,
			     const struct bpf_func_state *state)
{
	if (env->prev_log_len && env->prev_log_len == env->log.len_used) {
		/* remove new line character */
		bpf_vlog_reset(&env->log, env->prev_log_len - 1);
		verbose(env, "%*c;", vlog_alignment(env->prev_insn_print_len), ' ');
	} else {
		verbose(env, "%d:", env->insn_idx);
	}
	print_verifier_state(env, state, false);
}