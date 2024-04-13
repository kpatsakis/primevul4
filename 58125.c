static struct gs_tx_context *gs_get_tx_context(struct gs_can *dev,
					       unsigned int id)
{
	unsigned long flags;

	if (id < GS_MAX_TX_URBS) {
		spin_lock_irqsave(&dev->tx_ctx_lock, flags);
		if (dev->tx_context[id].echo_id == id) {
			spin_unlock_irqrestore(&dev->tx_ctx_lock, flags);
			return &dev->tx_context[id];
		}
		spin_unlock_irqrestore(&dev->tx_ctx_lock, flags);
	}
	return NULL;
}
