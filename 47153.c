static void oz_acquire_port(struct oz_port *port, void *hpd)
{
	INIT_LIST_HEAD(&port->isoc_out_ep);
	INIT_LIST_HEAD(&port->isoc_in_ep);
	port->flags |= OZ_PORT_F_PRESENT | OZ_PORT_F_CHANGED;
	port->status |= USB_PORT_STAT_CONNECTION |
			(USB_PORT_STAT_C_CONNECTION << 16);
	oz_usb_get(hpd);
	port->hpd = hpd;
}
