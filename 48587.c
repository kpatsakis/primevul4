static int pcrypt_do_parallel(struct padata_priv *padata, unsigned int *cb_cpu,
			      struct padata_pcrypt *pcrypt)
{
	unsigned int cpu_index, cpu, i;
	struct pcrypt_cpumask *cpumask;

	cpu = *cb_cpu;

	rcu_read_lock_bh();
	cpumask = rcu_dereference_bh(pcrypt->cb_cpumask);
	if (cpumask_test_cpu(cpu, cpumask->mask))
			goto out;

	if (!cpumask_weight(cpumask->mask))
			goto out;

	cpu_index = cpu % cpumask_weight(cpumask->mask);

	cpu = cpumask_first(cpumask->mask);
	for (i = 0; i < cpu_index; i++)
		cpu = cpumask_next(cpu, cpumask->mask);

	*cb_cpu = cpu;

out:
	rcu_read_unlock_bh();
	return padata_do_parallel(pcrypt->pinst, padata, cpu);
}
