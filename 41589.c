static int wdm_create(struct usb_interface *intf, struct usb_endpoint_descriptor *ep,
		u16 bufsize, int (*manage_power)(struct usb_interface *, int))
{
	int rv = -ENOMEM;
	struct wdm_device *desc;

	desc = kzalloc(sizeof(struct wdm_device), GFP_KERNEL);
	if (!desc)
		goto out;
	INIT_LIST_HEAD(&desc->device_list);
	mutex_init(&desc->rlock);
	mutex_init(&desc->wlock);
	spin_lock_init(&desc->iuspin);
	init_waitqueue_head(&desc->wait);
	desc->wMaxCommand = bufsize;
	/* this will be expanded and needed in hardware endianness */
	desc->inum = cpu_to_le16((u16)intf->cur_altsetting->desc.bInterfaceNumber);
	desc->intf = intf;
	INIT_WORK(&desc->rxwork, wdm_rxwork);

	rv = -EINVAL;
	if (!usb_endpoint_is_int_in(ep))
		goto err;

	desc->wMaxPacketSize = usb_endpoint_maxp(ep);

	desc->orq = kmalloc(sizeof(struct usb_ctrlrequest), GFP_KERNEL);
	if (!desc->orq)
		goto err;
	desc->irq = kmalloc(sizeof(struct usb_ctrlrequest), GFP_KERNEL);
	if (!desc->irq)
		goto err;

	desc->validity = usb_alloc_urb(0, GFP_KERNEL);
	if (!desc->validity)
		goto err;

	desc->response = usb_alloc_urb(0, GFP_KERNEL);
	if (!desc->response)
		goto err;

	desc->command = usb_alloc_urb(0, GFP_KERNEL);
	if (!desc->command)
		goto err;

	desc->ubuf = kmalloc(desc->wMaxCommand, GFP_KERNEL);
	if (!desc->ubuf)
		goto err;

	desc->sbuf = kmalloc(desc->wMaxPacketSize, GFP_KERNEL);
	if (!desc->sbuf)
		goto err;

	desc->inbuf = kmalloc(desc->wMaxCommand, GFP_KERNEL);
	if (!desc->inbuf)
		goto err;

	usb_fill_int_urb(
		desc->validity,
		interface_to_usbdev(intf),
		usb_rcvintpipe(interface_to_usbdev(intf), ep->bEndpointAddress),
		desc->sbuf,
		desc->wMaxPacketSize,
		wdm_int_callback,
		desc,
		ep->bInterval
	);

	desc->irq->bRequestType = (USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE);
	desc->irq->bRequest = USB_CDC_GET_ENCAPSULATED_RESPONSE;
	desc->irq->wValue = 0;
	desc->irq->wIndex = desc->inum;
	desc->irq->wLength = cpu_to_le16(desc->wMaxCommand);

	usb_fill_control_urb(
		desc->response,
		interface_to_usbdev(intf),
		/* using common endpoint 0 */
		usb_rcvctrlpipe(interface_to_usbdev(desc->intf), 0),
		(unsigned char *)desc->irq,
		desc->inbuf,
		desc->wMaxCommand,
		wdm_in_callback,
		desc
	);

	desc->manage_power = manage_power;

	spin_lock(&wdm_device_list_lock);
	list_add(&desc->device_list, &wdm_device_list);
	spin_unlock(&wdm_device_list_lock);

	rv = usb_register_dev(intf, &wdm_class);
	if (rv < 0)
		goto err;
	else
		dev_info(&intf->dev, "%s: USB WDM device\n", dev_name(intf->usb_dev));
out:
	return rv;
err:
	spin_lock(&wdm_device_list_lock);
	list_del(&desc->device_list);
	spin_unlock(&wdm_device_list_lock);
	cleanup(desc);
	return rv;
}
