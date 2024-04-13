static int tg3_rx_prodring_init(struct tg3 *tp,
				struct tg3_rx_prodring_set *tpr)
{
	tpr->rx_std_buffers = kzalloc(TG3_RX_STD_BUFF_RING_SIZE(tp),
				      GFP_KERNEL);
	if (!tpr->rx_std_buffers)
		return -ENOMEM;

	tpr->rx_std = dma_alloc_coherent(&tp->pdev->dev,
					 TG3_RX_STD_RING_BYTES(tp),
					 &tpr->rx_std_mapping,
					 GFP_KERNEL);
	if (!tpr->rx_std)
		goto err_out;

	if (tg3_flag(tp, JUMBO_CAPABLE) && !tg3_flag(tp, 5780_CLASS)) {
		tpr->rx_jmb_buffers = kzalloc(TG3_RX_JMB_BUFF_RING_SIZE(tp),
					      GFP_KERNEL);
		if (!tpr->rx_jmb_buffers)
			goto err_out;

		tpr->rx_jmb = dma_alloc_coherent(&tp->pdev->dev,
						 TG3_RX_JMB_RING_BYTES(tp),
						 &tpr->rx_jmb_mapping,
						 GFP_KERNEL);
		if (!tpr->rx_jmb)
			goto err_out;
	}

	return 0;

err_out:
	tg3_rx_prodring_fini(tp, tpr);
	return -ENOMEM;
}
