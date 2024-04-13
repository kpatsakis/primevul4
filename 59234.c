int perf_event_exit_cpu(unsigned int cpu)
{
	perf_event_exit_cpu_context(cpu);
	return 0;
}
