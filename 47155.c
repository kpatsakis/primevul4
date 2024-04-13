static int oz_build_endpoints_for_config(struct usb_hcd *hcd,
		struct oz_port *port, struct usb_host_config *config,
		gfp_t mem_flags)
{
	struct oz_hcd *ozhcd = port->ozhcd;
	int i;
	int num_iface = config->desc.bNumInterfaces;

	if (num_iface) {
		struct oz_interface *iface;

		iface = kmalloc_array(num_iface, sizeof(struct oz_interface),
					mem_flags | __GFP_ZERO);
		if (!iface)
			return -ENOMEM;
		spin_lock_bh(&ozhcd->hcd_lock);
		port->iface = iface;
		port->num_iface = num_iface;
		spin_unlock_bh(&ozhcd->hcd_lock);
	}
	for (i = 0; i < num_iface; i++) {
		struct usb_host_interface *intf =
			&config->intf_cache[i]->altsetting[0];
		if (oz_build_endpoints_for_interface(hcd, port, intf,
			mem_flags))
			goto fail;
	}
	return 0;
fail:
	oz_clean_endpoints_for_config(hcd, port);
	return -1;
}
