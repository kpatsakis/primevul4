int printk_needs_cpu(int cpu)
{
	if (cpu_is_offline(cpu))
		printk_tick();
	return __this_cpu_read(printk_pending);
}
