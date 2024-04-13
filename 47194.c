static int oz_plat_probe(struct platform_device *dev)
{
	int i;
	int err;
	struct usb_hcd *hcd;
	struct oz_hcd *ozhcd;

	hcd = usb_create_hcd(&g_oz_hc_drv, &dev->dev, dev_name(&dev->dev));
	if (hcd == NULL) {
		oz_dbg(ON, "Failed to created hcd object OK\n");
		return -ENOMEM;
	}
	ozhcd = oz_hcd_private(hcd);
	memset(ozhcd, 0, sizeof(*ozhcd));
	INIT_LIST_HEAD(&ozhcd->urb_pending_list);
	INIT_LIST_HEAD(&ozhcd->urb_cancel_list);
	INIT_LIST_HEAD(&ozhcd->orphanage);
	ozhcd->hcd = hcd;
	ozhcd->conn_port = -1;
	spin_lock_init(&ozhcd->hcd_lock);
	for (i = 0; i < OZ_NB_PORTS; i++) {
		struct oz_port *port = &ozhcd->ports[i];

		port->ozhcd = ozhcd;
		port->flags = 0;
		port->status = 0;
		port->bus_addr = 0xff;
		spin_lock_init(&port->port_lock);
	}
	err = usb_add_hcd(hcd, 0, 0);
	if (err) {
		oz_dbg(ON, "Failed to add hcd object OK\n");
		usb_put_hcd(hcd);
		return -1;
	}
	device_wakeup_enable(hcd->self.controller);

	spin_lock_bh(&g_hcdlock);
	g_ozhcd = ozhcd;
	spin_unlock_bh(&g_hcdlock);
	return 0;
}
