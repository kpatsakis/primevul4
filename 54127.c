static inline int dl_bw_cpus(int i)
{
	struct root_domain *rd = cpu_rq(i)->rd;
	int cpus = 0;

	for_each_cpu_and(i, rd->span, cpu_active_mask)
		cpus++;

	return cpus;
}
