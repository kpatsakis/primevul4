static int edge_get_icount(struct tty_struct *tty,
				struct serial_icounter_struct *icount)
{
	struct usb_serial_port *port = tty->driver_data;
	struct edgeport_port *edge_port = usb_get_serial_port_data(port);
	struct async_icount *ic = &edge_port->icount;

	icount->cts = ic->cts;
	icount->dsr = ic->dsr;
	icount->rng = ic->rng;
	icount->dcd = ic->dcd;
	icount->tx = ic->tx;
        icount->rx = ic->rx;
        icount->frame = ic->frame;
        icount->parity = ic->parity;
        icount->overrun = ic->overrun;
        icount->brk = ic->brk;
        icount->buf_overrun = ic->buf_overrun;
	return 0;
}
