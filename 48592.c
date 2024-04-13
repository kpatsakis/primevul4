static int pcrypt_init_padata(struct padata_pcrypt *pcrypt,
			      const char *name)
{
	int ret = -ENOMEM;
	struct pcrypt_cpumask *mask;

	get_online_cpus();

	pcrypt->wq = alloc_workqueue("%s", WQ_MEM_RECLAIM | WQ_CPU_INTENSIVE,
				     1, name);
	if (!pcrypt->wq)
		goto err;

	pcrypt->pinst = padata_alloc_possible(pcrypt->wq);
	if (!pcrypt->pinst)
		goto err_destroy_workqueue;

	mask = kmalloc(sizeof(*mask), GFP_KERNEL);
	if (!mask)
		goto err_free_padata;
	if (!alloc_cpumask_var(&mask->mask, GFP_KERNEL)) {
		kfree(mask);
		goto err_free_padata;
	}

	cpumask_and(mask->mask, cpu_possible_mask, cpu_online_mask);
	rcu_assign_pointer(pcrypt->cb_cpumask, mask);

	pcrypt->nblock.notifier_call = pcrypt_cpumask_change_notify;
	ret = padata_register_cpumask_notifier(pcrypt->pinst, &pcrypt->nblock);
	if (ret)
		goto err_free_cpumask;

	ret = pcrypt_sysfs_add(pcrypt->pinst, name);
	if (ret)
		goto err_unregister_notifier;

	put_online_cpus();

	return ret;

err_unregister_notifier:
	padata_unregister_cpumask_notifier(pcrypt->pinst, &pcrypt->nblock);
err_free_cpumask:
	free_cpumask_var(mask->mask);
	kfree(mask);
err_free_padata:
	padata_free(pcrypt->pinst);
err_destroy_workqueue:
	destroy_workqueue(pcrypt->wq);
err:
	put_online_cpus();

	return ret;
}
