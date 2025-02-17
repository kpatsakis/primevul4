static int gtco_probe(struct usb_interface *usbinterface,
		      const struct usb_device_id *id)
{

	struct gtco             *gtco;
	struct input_dev        *input_dev;
	struct hid_descriptor   *hid_desc;
	char                    *report;
	int                     result = 0, retry;
	int			error;
	struct usb_endpoint_descriptor *endpoint;
	struct usb_device	*udev = interface_to_usbdev(usbinterface);

	/* Allocate memory for device structure */
	gtco = kzalloc(sizeof(struct gtco), GFP_KERNEL);
	input_dev = input_allocate_device();
	if (!gtco || !input_dev) {
		dev_err(&usbinterface->dev, "No more memory\n");
		error = -ENOMEM;
		goto err_free_devs;
	}

	/* Set pointer to the input device */
	gtco->inputdevice = input_dev;

	/* Save interface information */
	gtco->intf = usbinterface;

	/* Allocate some data for incoming reports */
	gtco->buffer = usb_alloc_coherent(udev, REPORT_MAX_SIZE,
					  GFP_KERNEL, &gtco->buf_dma);
	if (!gtco->buffer) {
		dev_err(&usbinterface->dev, "No more memory for us buffers\n");
		error = -ENOMEM;
		goto err_free_devs;
	}

	/* Allocate URB for reports */
	gtco->urbinfo = usb_alloc_urb(0, GFP_KERNEL);
	if (!gtco->urbinfo) {
		dev_err(&usbinterface->dev, "Failed to allocate URB\n");
		error = -ENOMEM;
		goto err_free_buf;
	}

	/* Sanity check that a device has an endpoint */
	if (usbinterface->altsetting[0].desc.bNumEndpoints < 1) {
		dev_err(&usbinterface->dev,
			"Invalid number of endpoints\n");
		error = -EINVAL;
		goto err_free_urb;
	}

	/*
	 * The endpoint is always altsetting 0, we know this since we know
	 * this device only has one interrupt endpoint
	 */
	endpoint = &usbinterface->altsetting[0].endpoint[0].desc;

	/* Some debug */
	dev_dbg(&usbinterface->dev, "gtco # interfaces: %d\n", usbinterface->num_altsetting);
	dev_dbg(&usbinterface->dev, "num endpoints:     %d\n", usbinterface->cur_altsetting->desc.bNumEndpoints);
	dev_dbg(&usbinterface->dev, "interface class:   %d\n", usbinterface->cur_altsetting->desc.bInterfaceClass);
	dev_dbg(&usbinterface->dev, "endpoint: attribute:0x%x type:0x%x\n", endpoint->bmAttributes, endpoint->bDescriptorType);
	if (usb_endpoint_xfer_int(endpoint))
		dev_dbg(&usbinterface->dev, "endpoint: we have interrupt endpoint\n");

	dev_dbg(&usbinterface->dev, "endpoint extra len:%d\n", usbinterface->altsetting[0].extralen);

	/*
	 * Find the HID descriptor so we can find out the size of the
	 * HID report descriptor
	 */
	if (usb_get_extra_descriptor(usbinterface->cur_altsetting,
				     HID_DEVICE_TYPE, &hid_desc) != 0) {
		dev_err(&usbinterface->dev,
			"Can't retrieve exta USB descriptor to get hid report descriptor length\n");
		error = -EIO;
		goto err_free_urb;
	}

	dev_dbg(&usbinterface->dev,
		"Extra descriptor success: type:%d  len:%d\n",
		hid_desc->bDescriptorType,  hid_desc->wDescriptorLength);

	report = kzalloc(le16_to_cpu(hid_desc->wDescriptorLength), GFP_KERNEL);
	if (!report) {
		dev_err(&usbinterface->dev, "No more memory for report\n");
		error = -ENOMEM;
		goto err_free_urb;
	}

	/* Couple of tries to get reply */
	for (retry = 0; retry < 3; retry++) {
		result = usb_control_msg(udev,
					 usb_rcvctrlpipe(udev, 0),
					 USB_REQ_GET_DESCRIPTOR,
					 USB_RECIP_INTERFACE | USB_DIR_IN,
					 REPORT_DEVICE_TYPE << 8,
					 0, /* interface */
					 report,
					 le16_to_cpu(hid_desc->wDescriptorLength),
					 5000); /* 5 secs */

		dev_dbg(&usbinterface->dev, "usb_control_msg result: %d\n", result);
		if (result == le16_to_cpu(hid_desc->wDescriptorLength)) {
			parse_hid_report_descriptor(gtco, report, result);
			break;
		}
	}

	kfree(report);

	/* If we didn't get the report, fail */
	if (result != le16_to_cpu(hid_desc->wDescriptorLength)) {
		dev_err(&usbinterface->dev,
			"Failed to get HID Report Descriptor of size: %d\n",
			hid_desc->wDescriptorLength);
		error = -EIO;
		goto err_free_urb;
	}

	/* Create a device file node */
	usb_make_path(udev, gtco->usbpath, sizeof(gtco->usbpath));
	strlcat(gtco->usbpath, "/input0", sizeof(gtco->usbpath));

	/* Set Input device functions */
	input_dev->open = gtco_input_open;
	input_dev->close = gtco_input_close;

	/* Set input device information */
	input_dev->name = "GTCO_CalComp";
	input_dev->phys = gtco->usbpath;

	input_set_drvdata(input_dev, gtco);

	/* Now set up all the input device capabilities */
	gtco_setup_caps(input_dev);

	/* Set input device required ID information */
	usb_to_input_id(udev, &input_dev->id);
	input_dev->dev.parent = &usbinterface->dev;

	/* Setup the URB, it will be posted later on open of input device */
	endpoint = &usbinterface->altsetting[0].endpoint[0].desc;

	usb_fill_int_urb(gtco->urbinfo,
			 udev,
			 usb_rcvintpipe(udev,
					endpoint->bEndpointAddress),
			 gtco->buffer,
			 REPORT_MAX_SIZE,
			 gtco_urb_callback,
			 gtco,
			 endpoint->bInterval);

	gtco->urbinfo->transfer_dma = gtco->buf_dma;
	gtco->urbinfo->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

	/* Save gtco pointer in USB interface gtco */
	usb_set_intfdata(usbinterface, gtco);

	/* All done, now register the input device */
	error = input_register_device(input_dev);
	if (error)
		goto err_free_urb;

	return 0;

 err_free_urb:
	usb_free_urb(gtco->urbinfo);
 err_free_buf:
	usb_free_coherent(udev, REPORT_MAX_SIZE,
			  gtco->buffer, gtco->buf_dma);
 err_free_devs:
	input_free_device(input_dev);
	kfree(gtco);
	return error;
}
