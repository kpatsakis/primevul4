static int process_timer_func(struct bpf_verifier_env *env, int regno,
			      struct bpf_call_arg_meta *meta)
{
	struct bpf_reg_state *regs = cur_regs(env), *reg = &regs[regno];
	bool is_const = tnum_is_const(reg->var_off);
	struct bpf_map *map = reg->map_ptr;
	u64 val = reg->var_off.value;

	if (!is_const) {
		verbose(env,
			"R%d doesn't have constant offset. bpf_timer has to be at the constant offset\n",
			regno);
		return -EINVAL;
	}
	if (!map->btf) {
		verbose(env, "map '%s' has to have BTF in order to use bpf_timer\n",
			map->name);
		return -EINVAL;
	}
	if (!map_value_has_timer(map)) {
		if (map->timer_off == -E2BIG)
			verbose(env,
				"map '%s' has more than one 'struct bpf_timer'\n",
				map->name);
		else if (map->timer_off == -ENOENT)
			verbose(env,
				"map '%s' doesn't have 'struct bpf_timer'\n",
				map->name);
		else
			verbose(env,
				"map '%s' is not a struct type or bpf_timer is mangled\n",
				map->name);
		return -EINVAL;
	}
	if (map->timer_off != val + reg->off) {
		verbose(env, "off %lld doesn't point to 'struct bpf_timer' that is at %d\n",
			val + reg->off, map->timer_off);
		return -EINVAL;
	}
	if (meta->map_ptr) {
		verbose(env, "verifier bug. Two map pointers in a timer helper\n");
		return -EFAULT;
	}
	meta->map_uid = reg->map_uid;
	meta->map_ptr = map;
	return 0;
}