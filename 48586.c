static int pcrypt_cpumask_change_notify(struct notifier_block *self,
					unsigned long val, void *data)
{
	struct padata_pcrypt *pcrypt;
	struct pcrypt_cpumask *new_mask, *old_mask;
	struct padata_cpumask *cpumask = (struct padata_cpumask *)data;

	if (!(val & PADATA_CPU_SERIAL))
		return 0;

	pcrypt = container_of(self, struct padata_pcrypt, nblock);
	new_mask = kmalloc(sizeof(*new_mask), GFP_KERNEL);
	if (!new_mask)
		return -ENOMEM;
	if (!alloc_cpumask_var(&new_mask->mask, GFP_KERNEL)) {
		kfree(new_mask);
		return -ENOMEM;
	}

	old_mask = pcrypt->cb_cpumask;

	cpumask_copy(new_mask->mask, cpumask->cbcpu);
	rcu_assign_pointer(pcrypt->cb_cpumask, new_mask);
	synchronize_rcu_bh();

	free_cpumask_var(old_mask->mask);
	kfree(old_mask);
	return 0;
}
