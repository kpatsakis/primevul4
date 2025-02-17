static void pcrypt_fini_padata(struct padata_pcrypt *pcrypt)
{
	free_cpumask_var(pcrypt->cb_cpumask->mask);
	kfree(pcrypt->cb_cpumask);

	padata_stop(pcrypt->pinst);
	padata_unregister_cpumask_notifier(pcrypt->pinst, &pcrypt->nblock);
	destroy_workqueue(pcrypt->wq);
	padata_free(pcrypt->pinst);
}
