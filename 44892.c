static void put_reqs_available(struct kioctx *ctx, unsigned nr)
{
	struct kioctx_cpu *kcpu;

	preempt_disable();
	kcpu = this_cpu_ptr(ctx->cpu);

	kcpu->reqs_available += nr;
	while (kcpu->reqs_available >= ctx->req_batch * 2) {
		kcpu->reqs_available -= ctx->req_batch;
		atomic_add(ctx->req_batch, &ctx->reqs_available);
	}

	preempt_enable();
}
