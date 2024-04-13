static void mark_reg_unknown_value(struct bpf_reg_state *regs, u32 regno)
{
	BUG_ON(regno >= MAX_BPF_REG);
	__mark_reg_unknown_value(regs, regno);
}
