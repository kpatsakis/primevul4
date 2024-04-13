static irqreturn_t host_irq(struct ci13xxx *ci)
{
	return usb_hcd_irq(ci->irq, ci->hcd);
}
