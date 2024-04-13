static irqreturn_t tg3_msi(int irq, void *dev_id)
{
	struct tg3_napi *tnapi = dev_id;
	struct tg3 *tp = tnapi->tp;

	prefetch(tnapi->hw_status);
	if (tnapi->rx_rcb)
		prefetch(&tnapi->rx_rcb[tnapi->rx_rcb_ptr]);
	/*
	 * Writing any value to intr-mbox-0 clears PCI INTA# and
	 * chip-internal interrupt pending events.
	 * Writing non-zero to intr-mbox-0 additional tells the
	 * NIC to stop sending us irqs, engaging "in-intr-handler"
	 * event coalescing.
	 */
	tw32_mailbox(tnapi->int_mbox, 0x00000001);
	if (likely(!tg3_irq_sync(tp)))
		napi_schedule(&tnapi->napi);

	return IRQ_RETVAL(1);
}
