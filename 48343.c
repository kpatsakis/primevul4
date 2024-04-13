static int async_chainiv_schedule_work(struct async_chainiv_ctx *ctx)
{
	int queued;
	int err = ctx->err;

	if (!ctx->queue.qlen) {
		smp_mb__before_atomic();
		clear_bit(CHAINIV_STATE_INUSE, &ctx->state);

		if (!ctx->queue.qlen ||
		    test_and_set_bit(CHAINIV_STATE_INUSE, &ctx->state))
			goto out;
	}

	queued = queue_work(kcrypto_wq, &ctx->postponed);
	BUG_ON(!queued);

out:
	return err;
}
