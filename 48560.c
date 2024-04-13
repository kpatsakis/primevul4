static int mcryptd_init_queue(struct mcryptd_queue *queue,
			     unsigned int max_cpu_qlen)
{
	int cpu;
	struct mcryptd_cpu_queue *cpu_queue;

	queue->cpu_queue = alloc_percpu(struct mcryptd_cpu_queue);
	pr_debug("mqueue:%p mcryptd_cpu_queue %p\n", queue, queue->cpu_queue);
	if (!queue->cpu_queue)
		return -ENOMEM;
	for_each_possible_cpu(cpu) {
		cpu_queue = per_cpu_ptr(queue->cpu_queue, cpu);
		pr_debug("cpu_queue #%d %p\n", cpu, queue->cpu_queue);
		crypto_init_queue(&cpu_queue->queue, max_cpu_qlen);
		INIT_WORK(&cpu_queue->work, mcryptd_queue_worker);
	}
	return 0;
}
