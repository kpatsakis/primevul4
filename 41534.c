static irqreturn_t tg3_test_isr(int irq, void *dev_id)
{
	struct tg3_napi *tnapi = dev_id;
	struct tg3 *tp = tnapi->tp;
	struct tg3_hw_status *sblk = tnapi->hw_status;

	if ((sblk->status & SD_STATUS_UPDATED) ||
	    !(tr32(TG3PCI_PCISTATE) & PCISTATE_INT_NOT_ACTIVE)) {
		tg3_disable_ints(tp);
		return IRQ_RETVAL(1);
	}
	return IRQ_RETVAL(0);
}
