static void init_reg_state(struct bpf_reg_state *regs)
{
	int i;

	for (i = 0; i < MAX_BPF_REG; i++) {
		regs[i].type = NOT_INIT;
		regs[i].imm = 0;
		regs[i].min_value = BPF_REGISTER_MIN_RANGE;
		regs[i].max_value = BPF_REGISTER_MAX_RANGE;
	}

	/* frame pointer */
	regs[BPF_REG_FP].type = FRAME_PTR;

	/* 1st arg to a function */
	regs[BPF_REG_1].type = PTR_TO_CTX;
}
