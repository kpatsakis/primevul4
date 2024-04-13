static int mcryptd_enqueue_request(struct mcryptd_queue *queue,
				  struct crypto_async_request *request,
				  struct mcryptd_hash_request_ctx *rctx)
{
	int cpu, err;
	struct mcryptd_cpu_queue *cpu_queue;

	cpu = get_cpu();
	cpu_queue = this_cpu_ptr(queue->cpu_queue);
	rctx->tag.cpu = cpu;

	err = crypto_enqueue_request(&cpu_queue->queue, request);
	pr_debug("enqueue request: cpu %d cpu_queue %p request %p\n",
		 cpu, cpu_queue, request);
	queue_work_on(cpu, kcrypto_wq, &cpu_queue->work);
	put_cpu();

	return err;
}
