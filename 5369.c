static int check_stack_slot_within_bounds(int off,
					  struct bpf_func_state *state,
					  enum bpf_access_type t)
{
	int min_valid_off;

	if (t == BPF_WRITE)
		min_valid_off = -MAX_BPF_STACK;
	else
		min_valid_off = -state->allocated_stack;

	if (off < min_valid_off || off > -1)
		return -EACCES;
	return 0;
}