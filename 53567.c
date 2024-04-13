static void hub_port_logical_disconnect(struct usb_hub *hub, int port1)
{
	dev_dbg(&hub->ports[port1 - 1]->dev, "logical disconnect\n");
	hub_port_disable(hub, port1, 1);

	/* FIXME let caller ask to power down the port:
	 *  - some devices won't enumerate without a VBUS power cycle
	 *  - SRP saves power that way
	 *  - ... new call, TBD ...
	 * That's easy if this hub can switch power per-port, and
	 * hub_wq reactivates the port later (timer, SRP, etc).
	 * Powerdown must be optional, because of reset/DFU.
	 */

	set_bit(port1, hub->change_bits);
	kick_hub_wq(hub);
}
