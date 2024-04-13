static int edge_startup(struct usb_serial *serial)
{
	struct edgeport_serial *edge_serial;
	int status;
	u16 product_id;

	/* Make sure we have the required endpoints when in download mode. */
	if (serial->interface->cur_altsetting->desc.bNumEndpoints > 1) {
		if (serial->num_bulk_in < serial->num_ports ||
				serial->num_bulk_out < serial->num_ports)
			return -ENODEV;
	}

	/* create our private serial structure */
	edge_serial = kzalloc(sizeof(struct edgeport_serial), GFP_KERNEL);
	if (!edge_serial)
		return -ENOMEM;

	mutex_init(&edge_serial->es_lock);
	edge_serial->serial = serial;
	INIT_DELAYED_WORK(&edge_serial->heartbeat_work, edge_heartbeat_work);
	usb_set_serial_data(serial, edge_serial);

	status = download_fw(edge_serial);
	if (status < 0) {
		kfree(edge_serial);
		return status;
	}

	if (status > 0)
		return 1;	/* bind but do not register any ports */

	product_id = le16_to_cpu(
			edge_serial->serial->dev->descriptor.idProduct);

	/* Currently only the EP/416 models require heartbeat support */
	if (edge_serial->fw_version > FW_HEARTBEAT_VERSION_CUTOFF) {
		if (product_id == ION_DEVICE_ID_TI_EDGEPORT_416 ||
			product_id == ION_DEVICE_ID_TI_EDGEPORT_416B) {
			edge_serial->use_heartbeat = true;
		}
	}

	edge_heartbeat_schedule(edge_serial);

	return 0;
}
