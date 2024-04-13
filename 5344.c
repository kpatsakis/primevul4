static int process_spin_lock(struct bpf_verifier_env *env, int regno,
			     bool is_lock)
{
	struct bpf_reg_state *regs = cur_regs(env), *reg = &regs[regno];
	struct bpf_verifier_state *cur = env->cur_state;
	bool is_const = tnum_is_const(reg->var_off);
	struct bpf_map *map = reg->map_ptr;
	u64 val = reg->var_off.value;

	if (!is_const) {
		verbose(env,
			"R%d doesn't have constant offset. bpf_spin_lock has to be at the constant offset\n",
			regno);
		return -EINVAL;
	}
	if (!map->btf) {
		verbose(env,
			"map '%s' has to have BTF in order to use bpf_spin_lock\n",
			map->name);
		return -EINVAL;
	}
	if (!map_value_has_spin_lock(map)) {
		if (map->spin_lock_off == -E2BIG)
			verbose(env,
				"map '%s' has more than one 'struct bpf_spin_lock'\n",
				map->name);
		else if (map->spin_lock_off == -ENOENT)
			verbose(env,
				"map '%s' doesn't have 'struct bpf_spin_lock'\n",
				map->name);
		else
			verbose(env,
				"map '%s' is not a struct type or bpf_spin_lock is mangled\n",
				map->name);
		return -EINVAL;
	}
	if (map->spin_lock_off != val + reg->off) {
		verbose(env, "off %lld doesn't point to 'struct bpf_spin_lock'\n",
			val + reg->off);
		return -EINVAL;
	}
	if (is_lock) {
		if (cur->active_spin_lock) {
			verbose(env,
				"Locking two bpf_spin_locks are not allowed\n");
			return -EINVAL;
		}
		cur->active_spin_lock = reg->id;
	} else {
		if (!cur->active_spin_lock) {
			verbose(env, "bpf_spin_unlock without taking a lock\n");
			return -EINVAL;
		}
		if (cur->active_spin_lock != reg->id) {
			verbose(env, "bpf_spin_unlock of different lock\n");
			return -EINVAL;
		}
		cur->active_spin_lock = 0;
	}
	return 0;
}