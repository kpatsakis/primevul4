static inline int can_use_console(unsigned int cpu)
{
	return cpu_online(cpu) || have_callable_console();
}
