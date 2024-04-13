void start_thread_ia32(struct pt_regs *regs, u32 new_ip, u32 new_sp)
{
	start_thread_common(regs, new_ip, new_sp,
			    test_thread_flag(TIF_X32)
			    ? __USER_CS : __USER32_CS,
			    __USER_DS, __USER_DS);
}
