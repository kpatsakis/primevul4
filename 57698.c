static int check_packet_access(struct bpf_verifier_env *env, u32 regno, int off,
			       int size)
{
	struct bpf_reg_state *regs = env->cur_state.regs;
	struct bpf_reg_state *reg = &regs[regno];

	off += reg->off;
	if (off < 0 || size <= 0 || off + size > reg->range) {
		verbose("invalid access to packet, off=%d size=%d, R%d(id=%d,off=%d,r=%d)\n",
			off, size, regno, reg->id, reg->off, reg->range);
		return -EACCES;
	}
	return 0;
}
