static int tg3_abort_hw(struct tg3 *tp, int silent)
{
	int i, err;

	tg3_disable_ints(tp);

	tp->rx_mode &= ~RX_MODE_ENABLE;
	tw32_f(MAC_RX_MODE, tp->rx_mode);
	udelay(10);

	err  = tg3_stop_block(tp, RCVBDI_MODE, RCVBDI_MODE_ENABLE, silent);
	err |= tg3_stop_block(tp, RCVLPC_MODE, RCVLPC_MODE_ENABLE, silent);
	err |= tg3_stop_block(tp, RCVLSC_MODE, RCVLSC_MODE_ENABLE, silent);
	err |= tg3_stop_block(tp, RCVDBDI_MODE, RCVDBDI_MODE_ENABLE, silent);
	err |= tg3_stop_block(tp, RCVDCC_MODE, RCVDCC_MODE_ENABLE, silent);
	err |= tg3_stop_block(tp, RCVCC_MODE, RCVCC_MODE_ENABLE, silent);

	err |= tg3_stop_block(tp, SNDBDS_MODE, SNDBDS_MODE_ENABLE, silent);
	err |= tg3_stop_block(tp, SNDBDI_MODE, SNDBDI_MODE_ENABLE, silent);
	err |= tg3_stop_block(tp, SNDDATAI_MODE, SNDDATAI_MODE_ENABLE, silent);
	err |= tg3_stop_block(tp, RDMAC_MODE, RDMAC_MODE_ENABLE, silent);
	err |= tg3_stop_block(tp, SNDDATAC_MODE, SNDDATAC_MODE_ENABLE, silent);
	err |= tg3_stop_block(tp, DMAC_MODE, DMAC_MODE_ENABLE, silent);
	err |= tg3_stop_block(tp, SNDBDC_MODE, SNDBDC_MODE_ENABLE, silent);

	tp->mac_mode &= ~MAC_MODE_TDE_ENABLE;
	tw32_f(MAC_MODE, tp->mac_mode);
	udelay(40);

	tp->tx_mode &= ~TX_MODE_ENABLE;
	tw32_f(MAC_TX_MODE, tp->tx_mode);

	for (i = 0; i < MAX_WAIT_CNT; i++) {
		udelay(100);
		if (!(tr32(MAC_TX_MODE) & TX_MODE_ENABLE))
			break;
	}
	if (i >= MAX_WAIT_CNT) {
		dev_err(&tp->pdev->dev,
			"%s timed out, TX_MODE_ENABLE will not clear "
			"MAC_TX_MODE=%08x\n", __func__, tr32(MAC_TX_MODE));
		err |= -ENODEV;
	}

	err |= tg3_stop_block(tp, HOSTCC_MODE, HOSTCC_MODE_ENABLE, silent);
	err |= tg3_stop_block(tp, WDMAC_MODE, WDMAC_MODE_ENABLE, silent);
	err |= tg3_stop_block(tp, MBFREE_MODE, MBFREE_MODE_ENABLE, silent);

	tw32(FTQ_RESET, 0xffffffff);
	tw32(FTQ_RESET, 0x00000000);

	err |= tg3_stop_block(tp, BUFMGR_MODE, BUFMGR_MODE_ENABLE, silent);
	err |= tg3_stop_block(tp, MEMARB_MODE, MEMARB_MODE_ENABLE, silent);

	for (i = 0; i < tp->irq_cnt; i++) {
		struct tg3_napi *tnapi = &tp->napi[i];
		if (tnapi->hw_status)
			memset(tnapi->hw_status, 0, TG3_HW_STATUS_SIZE);
	}

	return err;
}
