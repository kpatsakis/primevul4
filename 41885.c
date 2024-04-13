static void handle_new_msr(struct edgeport_port *edge_port, __u8 msr)
{
	struct async_icount *icount;
	struct tty_struct *tty;

	dev_dbg(&edge_port->port->dev, "%s - %02x\n", __func__, msr);

	if (msr & (EDGEPORT_MSR_DELTA_CTS | EDGEPORT_MSR_DELTA_DSR |
			EDGEPORT_MSR_DELTA_RI | EDGEPORT_MSR_DELTA_CD)) {
		icount = &edge_port->icount;

		/* update input line counters */
		if (msr & EDGEPORT_MSR_DELTA_CTS)
			icount->cts++;
		if (msr & EDGEPORT_MSR_DELTA_DSR)
			icount->dsr++;
		if (msr & EDGEPORT_MSR_DELTA_CD)
			icount->dcd++;
		if (msr & EDGEPORT_MSR_DELTA_RI)
			icount->rng++;
		wake_up_interruptible(&edge_port->delta_msr_wait);
	}

	/* Save the new modem status */
	edge_port->shadow_msr = msr & 0xf0;

	tty = tty_port_tty_get(&edge_port->port->port);
	/* handle CTS flow control */
	if (tty && C_CRTSCTS(tty)) {
		if (msr & EDGEPORT_MSR_CTS) {
			tty->hw_stopped = 0;
			tty_wakeup(tty);
		} else {
			tty->hw_stopped = 1;
		}
	}
	tty_kref_put(tty);
}
