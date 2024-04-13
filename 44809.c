static irqreturn_t wanxl_intr(int irq, void* dev_id)
{
        card_t *card = dev_id;
        int i;
        u32 stat;
        int handled = 0;


        while((stat = readl(card->plx + PLX_DOORBELL_FROM_CARD)) != 0) {
                handled = 1;
		writel(stat, card->plx + PLX_DOORBELL_FROM_CARD);

                for (i = 0; i < card->n_ports; i++) {
			if (stat & (1 << (DOORBELL_FROM_CARD_TX_0 + i)))
				wanxl_tx_intr(&card->ports[i]);
			if (stat & (1 << (DOORBELL_FROM_CARD_CABLE_0 + i)))
				wanxl_cable_intr(&card->ports[i]);
		}
		if (stat & (1 << DOORBELL_FROM_CARD_RX))
			wanxl_rx_intr(card);
        }

        return IRQ_RETVAL(handled);
}
