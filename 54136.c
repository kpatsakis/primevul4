static int get_group(int cpu, struct sd_data *sdd, struct sched_group **sg)
{
	struct sched_domain *sd = *per_cpu_ptr(sdd->sd, cpu);
	struct sched_domain *child = sd->child;

	if (child)
		cpu = cpumask_first(sched_domain_span(child));

	if (sg) {
		*sg = *per_cpu_ptr(sdd->sg, cpu);
		(*sg)->sgp = *per_cpu_ptr(sdd->sgp, cpu);
		atomic_set(&(*sg)->sgp->ref, 1); /* for claim_allocations */
	}

	return cpu;
}
