static int check_map_prog_compatibility(struct bpf_verifier_env *env,
					struct bpf_map *map,
					struct bpf_prog *prog)

{
	enum bpf_prog_type prog_type = resolve_prog_type(prog);
	/*
	 * Validate that trace type programs use preallocated hash maps.
	 *
	 * For programs attached to PERF events this is mandatory as the
	 * perf NMI can hit any arbitrary code sequence.
	 *
	 * All other trace types using preallocated hash maps are unsafe as
	 * well because tracepoint or kprobes can be inside locked regions
	 * of the memory allocator or at a place where a recursion into the
	 * memory allocator would see inconsistent state.
	 *
	 * On RT enabled kernels run-time allocation of all trace type
	 * programs is strictly prohibited due to lock type constraints. On
	 * !RT kernels it is allowed for backwards compatibility reasons for
	 * now, but warnings are emitted so developers are made aware of
	 * the unsafety and can fix their programs before this is enforced.
	 */
	if (is_tracing_prog_type(prog_type) && !is_preallocated_map(map)) {
		if (prog_type == BPF_PROG_TYPE_PERF_EVENT) {
			verbose(env, "perf_event programs can only use preallocated hash map\n");
			return -EINVAL;
		}
		if (IS_ENABLED(CONFIG_PREEMPT_RT)) {
			verbose(env, "trace type programs can only use preallocated hash map\n");
			return -EINVAL;
		}
		WARN_ONCE(1, "trace type BPF program uses run-time allocation\n");
		verbose(env, "trace type programs with run-time allocated hash maps are unsafe. Switch to preallocated hash maps.\n");
	}

	if (map_value_has_spin_lock(map)) {
		if (prog_type == BPF_PROG_TYPE_SOCKET_FILTER) {
			verbose(env, "socket filter progs cannot use bpf_spin_lock yet\n");
			return -EINVAL;
		}

		if (is_tracing_prog_type(prog_type)) {
			verbose(env, "tracing progs cannot use bpf_spin_lock yet\n");
			return -EINVAL;
		}

		if (prog->aux->sleepable) {
			verbose(env, "sleepable progs cannot use bpf_spin_lock yet\n");
			return -EINVAL;
		}
	}

	if (map_value_has_timer(map)) {
		if (is_tracing_prog_type(prog_type)) {
			verbose(env, "tracing progs cannot use bpf_timer yet\n");
			return -EINVAL;
		}
	}

	if ((bpf_prog_is_dev_bound(prog->aux) || bpf_map_is_dev_bound(map)) &&
	    !bpf_offload_prog_map_match(prog, map)) {
		verbose(env, "offload device mismatch between prog and map\n");
		return -EINVAL;
	}

	if (map->map_type == BPF_MAP_TYPE_STRUCT_OPS) {
		verbose(env, "bpf_struct_ops map cannot be used in prog\n");
		return -EINVAL;
	}

	if (prog->aux->sleepable)
		switch (map->map_type) {
		case BPF_MAP_TYPE_HASH:
		case BPF_MAP_TYPE_LRU_HASH:
		case BPF_MAP_TYPE_ARRAY:
		case BPF_MAP_TYPE_PERCPU_HASH:
		case BPF_MAP_TYPE_PERCPU_ARRAY:
		case BPF_MAP_TYPE_LRU_PERCPU_HASH:
		case BPF_MAP_TYPE_ARRAY_OF_MAPS:
		case BPF_MAP_TYPE_HASH_OF_MAPS:
			if (!is_preallocated_map(map)) {
				verbose(env,
					"Sleepable programs can only use preallocated maps\n");
				return -EINVAL;
			}
			break;
		case BPF_MAP_TYPE_RINGBUF:
		case BPF_MAP_TYPE_INODE_STORAGE:
		case BPF_MAP_TYPE_SK_STORAGE:
		case BPF_MAP_TYPE_TASK_STORAGE:
			break;
		default:
			verbose(env,
				"Sleepable programs can only use array, hash, and ringbuf maps\n");
			return -EINVAL;
		}

	return 0;
}