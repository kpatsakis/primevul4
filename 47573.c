print_ucode_info(struct ucode_cpu_info *uci, unsigned int date)
{
	int cpu = smp_processor_id();

	pr_info("CPU%d microcode updated early to revision 0x%x, date = %04x-%02x-%02x\n",
		cpu,
		uci->cpu_sig.rev,
		date & 0xffff,
		date >> 24,
		(date >> 16) & 0xff);
}
