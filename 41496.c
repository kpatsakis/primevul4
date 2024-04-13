static void tg3_rx_prodring_fini(struct tg3 *tp,
				 struct tg3_rx_prodring_set *tpr)
{
	kfree(tpr->rx_std_buffers);
	tpr->rx_std_buffers = NULL;
	kfree(tpr->rx_jmb_buffers);
	tpr->rx_jmb_buffers = NULL;
	if (tpr->rx_std) {
		dma_free_coherent(&tp->pdev->dev, TG3_RX_STD_RING_BYTES(tp),
				  tpr->rx_std, tpr->rx_std_mapping);
		tpr->rx_std = NULL;
	}
	if (tpr->rx_jmb) {
		dma_free_coherent(&tp->pdev->dev, TG3_RX_JMB_RING_BYTES(tp),
				  tpr->rx_jmb, tpr->rx_jmb_mapping);
		tpr->rx_jmb = NULL;
	}
}
