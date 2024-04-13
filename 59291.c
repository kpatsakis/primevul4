static void swevent_hlist_put(void)
{
	int cpu;

	for_each_possible_cpu(cpu)
		swevent_hlist_put_cpu(cpu);
}
