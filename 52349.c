static void cdc_ncm_txpath_bh(unsigned long param)
{
	struct usbnet *dev = (struct usbnet *)param;
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];

	spin_lock_bh(&ctx->mtx);
	if (ctx->tx_timer_pending != 0) {
		ctx->tx_timer_pending--;
		cdc_ncm_tx_timeout_start(ctx);
		spin_unlock_bh(&ctx->mtx);
	} else if (dev->net != NULL) {
		ctx->tx_reason_timeout++;	/* count reason for transmitting */
		spin_unlock_bh(&ctx->mtx);
		netif_tx_lock_bh(dev->net);
		usbnet_start_xmit(NULL, dev->net);
		netif_tx_unlock_bh(dev->net);
	} else {
		spin_unlock_bh(&ctx->mtx);
	}
}
