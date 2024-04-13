static int check_map_access(struct bpf_verifier_env *env, u32 regno, int off,
			    int size)
{
	struct bpf_map *map = env->cur_state.regs[regno].map_ptr;

	if (off < 0 || size <= 0 || off + size > map->value_size) {
		verbose("invalid access to map value, value_size=%d off=%d size=%d\n",
			map->value_size, off, size);
		return -EACCES;
	}
	return 0;
}
