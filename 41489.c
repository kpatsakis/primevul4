static void tg3_rings_reset(struct tg3 *tp)
{
	int i;
	u32 stblk, txrcb, rxrcb, limit;
	struct tg3_napi *tnapi = &tp->napi[0];

	/* Disable all transmit rings but the first. */
	if (!tg3_flag(tp, 5705_PLUS))
		limit = NIC_SRAM_SEND_RCB + TG3_BDINFO_SIZE * 16;
	else if (tg3_flag(tp, 5717_PLUS))
		limit = NIC_SRAM_SEND_RCB + TG3_BDINFO_SIZE * 4;
	else if (tg3_flag(tp, 57765_CLASS) ||
		 tg3_asic_rev(tp) == ASIC_REV_5762)
		limit = NIC_SRAM_SEND_RCB + TG3_BDINFO_SIZE * 2;
	else
		limit = NIC_SRAM_SEND_RCB + TG3_BDINFO_SIZE;

	for (txrcb = NIC_SRAM_SEND_RCB + TG3_BDINFO_SIZE;
	     txrcb < limit; txrcb += TG3_BDINFO_SIZE)
		tg3_write_mem(tp, txrcb + TG3_BDINFO_MAXLEN_FLAGS,
			      BDINFO_FLAGS_DISABLED);


	/* Disable all receive return rings but the first. */
	if (tg3_flag(tp, 5717_PLUS))
		limit = NIC_SRAM_RCV_RET_RCB + TG3_BDINFO_SIZE * 17;
	else if (!tg3_flag(tp, 5705_PLUS))
		limit = NIC_SRAM_RCV_RET_RCB + TG3_BDINFO_SIZE * 16;
	else if (tg3_asic_rev(tp) == ASIC_REV_5755 ||
		 tg3_asic_rev(tp) == ASIC_REV_5762 ||
		 tg3_flag(tp, 57765_CLASS))
		limit = NIC_SRAM_RCV_RET_RCB + TG3_BDINFO_SIZE * 4;
	else
		limit = NIC_SRAM_RCV_RET_RCB + TG3_BDINFO_SIZE;

	for (rxrcb = NIC_SRAM_RCV_RET_RCB + TG3_BDINFO_SIZE;
	     rxrcb < limit; rxrcb += TG3_BDINFO_SIZE)
		tg3_write_mem(tp, rxrcb + TG3_BDINFO_MAXLEN_FLAGS,
			      BDINFO_FLAGS_DISABLED);

	/* Disable interrupts */
	tw32_mailbox_f(tp->napi[0].int_mbox, 1);
	tp->napi[0].chk_msi_cnt = 0;
	tp->napi[0].last_rx_cons = 0;
	tp->napi[0].last_tx_cons = 0;

	/* Zero mailbox registers. */
	if (tg3_flag(tp, SUPPORT_MSIX)) {
		for (i = 1; i < tp->irq_max; i++) {
			tp->napi[i].tx_prod = 0;
			tp->napi[i].tx_cons = 0;
			if (tg3_flag(tp, ENABLE_TSS))
				tw32_mailbox(tp->napi[i].prodmbox, 0);
			tw32_rx_mbox(tp->napi[i].consmbox, 0);
			tw32_mailbox_f(tp->napi[i].int_mbox, 1);
			tp->napi[i].chk_msi_cnt = 0;
			tp->napi[i].last_rx_cons = 0;
			tp->napi[i].last_tx_cons = 0;
		}
		if (!tg3_flag(tp, ENABLE_TSS))
			tw32_mailbox(tp->napi[0].prodmbox, 0);
	} else {
		tp->napi[0].tx_prod = 0;
		tp->napi[0].tx_cons = 0;
		tw32_mailbox(tp->napi[0].prodmbox, 0);
		tw32_rx_mbox(tp->napi[0].consmbox, 0);
	}

	/* Make sure the NIC-based send BD rings are disabled. */
	if (!tg3_flag(tp, 5705_PLUS)) {
		u32 mbox = MAILBOX_SNDNIC_PROD_IDX_0 + TG3_64BIT_REG_LOW;
		for (i = 0; i < 16; i++)
			tw32_tx_mbox(mbox + i * 8, 0);
	}

	txrcb = NIC_SRAM_SEND_RCB;
	rxrcb = NIC_SRAM_RCV_RET_RCB;

	/* Clear status block in ram. */
	memset(tnapi->hw_status, 0, TG3_HW_STATUS_SIZE);

	/* Set status block DMA address */
	tw32(HOSTCC_STATUS_BLK_HOST_ADDR + TG3_64BIT_REG_HIGH,
	     ((u64) tnapi->status_mapping >> 32));
	tw32(HOSTCC_STATUS_BLK_HOST_ADDR + TG3_64BIT_REG_LOW,
	     ((u64) tnapi->status_mapping & 0xffffffff));

	if (tnapi->tx_ring) {
		tg3_set_bdinfo(tp, txrcb, tnapi->tx_desc_mapping,
			       (TG3_TX_RING_SIZE <<
				BDINFO_FLAGS_MAXLEN_SHIFT),
			       NIC_SRAM_TX_BUFFER_DESC);
		txrcb += TG3_BDINFO_SIZE;
	}

	if (tnapi->rx_rcb) {
		tg3_set_bdinfo(tp, rxrcb, tnapi->rx_rcb_mapping,
			       (tp->rx_ret_ring_mask + 1) <<
				BDINFO_FLAGS_MAXLEN_SHIFT, 0);
		rxrcb += TG3_BDINFO_SIZE;
	}

	stblk = HOSTCC_STATBLCK_RING1;

	for (i = 1, tnapi++; i < tp->irq_cnt; i++, tnapi++) {
		u64 mapping = (u64)tnapi->status_mapping;
		tw32(stblk + TG3_64BIT_REG_HIGH, mapping >> 32);
		tw32(stblk + TG3_64BIT_REG_LOW, mapping & 0xffffffff);

		/* Clear status block in ram. */
		memset(tnapi->hw_status, 0, TG3_HW_STATUS_SIZE);

		if (tnapi->tx_ring) {
			tg3_set_bdinfo(tp, txrcb, tnapi->tx_desc_mapping,
				       (TG3_TX_RING_SIZE <<
					BDINFO_FLAGS_MAXLEN_SHIFT),
				       NIC_SRAM_TX_BUFFER_DESC);
			txrcb += TG3_BDINFO_SIZE;
		}

		tg3_set_bdinfo(tp, rxrcb, tnapi->rx_rcb_mapping,
			       ((tp->rx_ret_ring_mask + 1) <<
				BDINFO_FLAGS_MAXLEN_SHIFT), 0);

		stblk += 8;
		rxrcb += TG3_BDINFO_SIZE;
	}
}
