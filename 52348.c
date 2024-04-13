static enum hrtimer_restart cdc_ncm_tx_timer_cb(struct hrtimer *timer)
{
	struct cdc_ncm_ctx *ctx =
			container_of(timer, struct cdc_ncm_ctx, tx_timer);

	if (!atomic_read(&ctx->stop))
		tasklet_schedule(&ctx->bh);
	return HRTIMER_NORESTART;
}
