iperf_setaffinity(int affinity)
{
#ifdef linux
    cpu_set_t cpu_set;

    CPU_ZERO(&cpu_set);
    CPU_SET(affinity, &cpu_set);
    if (sched_setaffinity(0, sizeof(cpu_set_t), &cpu_set) != 0) {
	i_errno = IEAFFINITY;
        return -1;
    }
    return 0;
#else /*linux*/
    i_errno = IEAFFINITY;
    return -1;
#endif /*linux*/
}
