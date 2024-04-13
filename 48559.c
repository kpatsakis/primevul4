static int __init mcryptd_init(void)
{
	int err, cpu;
	struct mcryptd_flush_list *flist;

	mcryptd_flist = alloc_percpu(struct mcryptd_flush_list);
	for_each_possible_cpu(cpu) {
		flist = per_cpu_ptr(mcryptd_flist, cpu);
		INIT_LIST_HEAD(&flist->list);
		mutex_init(&flist->lock);
	}

	err = mcryptd_init_queue(&mqueue, MCRYPTD_MAX_CPU_QLEN);
	if (err) {
		free_percpu(mcryptd_flist);
		return err;
	}

	err = crypto_register_template(&mcryptd_tmpl);
	if (err) {
		mcryptd_fini_queue(&mqueue);
		free_percpu(mcryptd_flist);
	}

	return err;
}
