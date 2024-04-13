static int edge_tiocmget(struct tty_struct *tty)
{
	struct usb_serial_port *port = tty->driver_data;
	struct edgeport_port *edge_port = usb_get_serial_port_data(port);
	unsigned int result = 0;
	unsigned int msr;
	unsigned int mcr;
	unsigned long flags;

	spin_lock_irqsave(&edge_port->ep_lock, flags);

	msr = edge_port->shadow_msr;
	mcr = edge_port->shadow_mcr;
	result = ((mcr & MCR_DTR)	? TIOCM_DTR: 0)	  /* 0x002 */
		  | ((mcr & MCR_RTS)	? TIOCM_RTS: 0)   /* 0x004 */
		  | ((msr & EDGEPORT_MSR_CTS)	? TIOCM_CTS: 0)   /* 0x020 */
		  | ((msr & EDGEPORT_MSR_CD)	? TIOCM_CAR: 0)   /* 0x040 */
		  | ((msr & EDGEPORT_MSR_RI)	? TIOCM_RI:  0)   /* 0x080 */
		  | ((msr & EDGEPORT_MSR_DSR)	? TIOCM_DSR: 0);  /* 0x100 */


	dev_dbg(&port->dev, "%s -- %x\n", __func__, result);
	spin_unlock_irqrestore(&edge_port->ep_lock, flags);

	return result;
}
