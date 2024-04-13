static int check_mem_access(struct bpf_verifier_env *env, u32 regno, int off,
			    int bpf_size, enum bpf_access_type t,
			    int value_regno)
{
	struct bpf_verifier_state *state = &env->cur_state;
	struct bpf_reg_state *reg = &state->regs[regno];
	int size, err = 0;

	if (reg->type == PTR_TO_STACK)
		off += reg->imm;

	size = bpf_size_to_bytes(bpf_size);
	if (size < 0)
		return size;

	err = check_ptr_alignment(reg, off, size);
	if (err)
		return err;

	if (reg->type == PTR_TO_MAP_VALUE ||
	    reg->type == PTR_TO_MAP_VALUE_ADJ) {
		if (t == BPF_WRITE && value_regno >= 0 &&
		    is_pointer_value(env, value_regno)) {
			verbose("R%d leaks addr into map\n", value_regno);
			return -EACCES;
		}

		if (reg->type == PTR_TO_MAP_VALUE_ADJ)
			err = check_map_access_adj(env, regno, off, size);
		else
			err = check_map_access(env, regno, off, size);
		if (!err && t == BPF_READ && value_regno >= 0)
			mark_reg_unknown_value_and_range(state->regs,
							 value_regno);

	} else if (reg->type == PTR_TO_CTX) {
		enum bpf_reg_type reg_type = UNKNOWN_VALUE;

		if (t == BPF_WRITE && value_regno >= 0 &&
		    is_pointer_value(env, value_regno)) {
			verbose("R%d leaks addr into ctx\n", value_regno);
			return -EACCES;
		}
		err = check_ctx_access(env, off, size, t, &reg_type);
		if (!err && t == BPF_READ && value_regno >= 0) {
			mark_reg_unknown_value_and_range(state->regs,
							 value_regno);
			/* note that reg.[id|off|range] == 0 */
			state->regs[value_regno].type = reg_type;
		}

	} else if (reg->type == FRAME_PTR || reg->type == PTR_TO_STACK) {
		if (off >= 0 || off < -MAX_BPF_STACK) {
			verbose("invalid stack off=%d size=%d\n", off, size);
			return -EACCES;
		}
		if (t == BPF_WRITE) {
			if (!env->allow_ptr_leaks &&
			    state->stack_slot_type[MAX_BPF_STACK + off] == STACK_SPILL &&
			    size != BPF_REG_SIZE) {
				verbose("attempt to corrupt spilled pointer on stack\n");
				return -EACCES;
			}
			err = check_stack_write(state, off, size, value_regno);
		} else {
			err = check_stack_read(state, off, size, value_regno);
		}
	} else if (state->regs[regno].type == PTR_TO_PACKET) {
		if (t == BPF_WRITE && !may_access_direct_pkt_data(env, NULL, t)) {
			verbose("cannot write into packet\n");
			return -EACCES;
		}
		if (t == BPF_WRITE && value_regno >= 0 &&
		    is_pointer_value(env, value_regno)) {
			verbose("R%d leaks addr into packet\n", value_regno);
			return -EACCES;
		}
		err = check_packet_access(env, regno, off, size);
		if (!err && t == BPF_READ && value_regno >= 0)
			mark_reg_unknown_value_and_range(state->regs,
							 value_regno);
	} else {
		verbose("R%d invalid mem access '%s'\n",
			regno, reg_type_str[reg->type]);
		return -EACCES;
	}

	if (!err && size <= 2 && value_regno >= 0 && env->allow_ptr_leaks &&
	    state->regs[value_regno].type == UNKNOWN_VALUE) {
		/* 1 or 2 byte load zero-extends, determine the number of
		 * zero upper bits. Not doing it fo 4 byte load, since
		 * such values cannot be added to ptr_to_packet anyway.
		 */
		state->regs[value_regno].imm = 64 - size * 8;
	}
	return err;
}
