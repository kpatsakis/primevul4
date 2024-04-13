static void release_btfs(struct bpf_verifier_env *env)
{
	__bpf_free_used_btfs(env->prog->aux, env->used_btfs,
			     env->used_btf_cnt);
}