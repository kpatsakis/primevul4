static int __sdt_alloc(const struct cpumask *cpu_map)
{
	struct sched_domain_topology_level *tl;
	int j;

	for_each_sd_topology(tl) {
		struct sd_data *sdd = &tl->data;

		sdd->sd = alloc_percpu(struct sched_domain *);
		if (!sdd->sd)
			return -ENOMEM;

		sdd->sg = alloc_percpu(struct sched_group *);
		if (!sdd->sg)
			return -ENOMEM;

		sdd->sgp = alloc_percpu(struct sched_group_power *);
		if (!sdd->sgp)
			return -ENOMEM;

		for_each_cpu(j, cpu_map) {
			struct sched_domain *sd;
			struct sched_group *sg;
			struct sched_group_power *sgp;

		       	sd = kzalloc_node(sizeof(struct sched_domain) + cpumask_size(),
					GFP_KERNEL, cpu_to_node(j));
			if (!sd)
				return -ENOMEM;

			*per_cpu_ptr(sdd->sd, j) = sd;

			sg = kzalloc_node(sizeof(struct sched_group) + cpumask_size(),
					GFP_KERNEL, cpu_to_node(j));
			if (!sg)
				return -ENOMEM;

			sg->next = sg;

			*per_cpu_ptr(sdd->sg, j) = sg;

			sgp = kzalloc_node(sizeof(struct sched_group_power) + cpumask_size(),
					GFP_KERNEL, cpu_to_node(j));
			if (!sgp)
				return -ENOMEM;

			*per_cpu_ptr(sdd->sgp, j) = sgp;
		}
	}

	return 0;
}
