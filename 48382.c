static int cryptd_enqueue_request(struct cryptd_queue *queue,
				  struct crypto_async_request *request)
{
	int cpu, err;
	struct cryptd_cpu_queue *cpu_queue;

	cpu = get_cpu();
	cpu_queue = this_cpu_ptr(queue->cpu_queue);
	err = crypto_enqueue_request(&cpu_queue->queue, request);
	queue_work_on(cpu, kcrypto_wq, &cpu_queue->work);
	put_cpu();

	return err;
}
