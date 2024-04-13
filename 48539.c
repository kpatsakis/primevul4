static void mcryptd_fini_queue(struct mcryptd_queue *queue)
{
	int cpu;
	struct mcryptd_cpu_queue *cpu_queue;

	for_each_possible_cpu(cpu) {
		cpu_queue = per_cpu_ptr(queue->cpu_queue, cpu);
		BUG_ON(cpu_queue->queue.qlen);
	}
	free_percpu(queue->cpu_queue);
}
