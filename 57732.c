static void reset_reg_range_values(struct bpf_reg_state *regs, u32 regno)
{
	regs[regno].min_value = BPF_REGISTER_MIN_RANGE;
	regs[regno].max_value = BPF_REGISTER_MAX_RANGE;
}
