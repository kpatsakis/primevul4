static void mark_reg_unknown_value_and_range(struct bpf_reg_state *regs,
					     u32 regno)
{
	mark_reg_unknown_value(regs, regno);
	reset_reg_range_values(regs, regno);
}
