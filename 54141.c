static void init_sched_groups_power(int cpu, struct sched_domain *sd)
{
	struct sched_group *sg = sd->groups;

	WARN_ON(!sg);

	do {
		sg->group_weight = cpumask_weight(sched_group_cpus(sg));
		sg = sg->next;
	} while (sg != sd->groups);

	if (cpu != group_balance_cpu(sg))
		return;

	update_group_power(sd, cpu);
	atomic_set(&sg->sgp->nr_busy_cpus, sg->group_weight);
}
