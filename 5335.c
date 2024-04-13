static int check_map_access(struct bpf_verifier_env *env, u32 regno,
			    int off, int size, bool zero_size_allowed)
{
	struct bpf_verifier_state *vstate = env->cur_state;
	struct bpf_func_state *state = vstate->frame[vstate->curframe];
	struct bpf_reg_state *reg = &state->regs[regno];
	struct bpf_map *map = reg->map_ptr;
	int err;

	err = check_mem_region_access(env, regno, off, size, map->value_size,
				      zero_size_allowed);
	if (err)
		return err;

	if (map_value_has_spin_lock(map)) {
		u32 lock = map->spin_lock_off;

		/* if any part of struct bpf_spin_lock can be touched by
		 * load/store reject this program.
		 * To check that [x1, x2) overlaps with [y1, y2)
		 * it is sufficient to check x1 < y2 && y1 < x2.
		 */
		if (reg->smin_value + off < lock + sizeof(struct bpf_spin_lock) &&
		     lock < reg->umax_value + off + size) {
			verbose(env, "bpf_spin_lock cannot be accessed directly by load/store\n");
			return -EACCES;
		}
	}
	if (map_value_has_timer(map)) {
		u32 t = map->timer_off;

		if (reg->smin_value + off < t + sizeof(struct bpf_timer) &&
		     t < reg->umax_value + off + size) {
			verbose(env, "bpf_timer cannot be accessed directly by load/store\n");
			return -EACCES;
		}
	}
	return err;
}