static int ims_pcu_buffers_alloc(struct ims_pcu *pcu)
{
	int error;

	pcu->urb_in_buf = usb_alloc_coherent(pcu->udev, pcu->max_in_size,
					     GFP_KERNEL, &pcu->read_dma);
	if (!pcu->urb_in_buf) {
		dev_err(pcu->dev,
			"Failed to allocate memory for read buffer\n");
		return -ENOMEM;
	}

	pcu->urb_in = usb_alloc_urb(0, GFP_KERNEL);
	if (!pcu->urb_in) {
		dev_err(pcu->dev, "Failed to allocate input URB\n");
		error = -ENOMEM;
		goto err_free_urb_in_buf;
	}

	pcu->urb_in->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
	pcu->urb_in->transfer_dma = pcu->read_dma;

	usb_fill_bulk_urb(pcu->urb_in, pcu->udev,
			  usb_rcvbulkpipe(pcu->udev,
					  pcu->ep_in->bEndpointAddress),
			  pcu->urb_in_buf, pcu->max_in_size,
			  ims_pcu_irq, pcu);

	/*
	 * We are using usb_bulk_msg() for sending so there is no point
	 * in allocating memory with usb_alloc_coherent().
	 */
	pcu->urb_out_buf = kmalloc(pcu->max_out_size, GFP_KERNEL);
	if (!pcu->urb_out_buf) {
		dev_err(pcu->dev, "Failed to allocate memory for write buffer\n");
		error = -ENOMEM;
		goto err_free_in_urb;
	}

	pcu->urb_ctrl_buf = usb_alloc_coherent(pcu->udev, pcu->max_ctrl_size,
					       GFP_KERNEL, &pcu->ctrl_dma);
	if (!pcu->urb_ctrl_buf) {
		dev_err(pcu->dev,
			"Failed to allocate memory for read buffer\n");
		error = -ENOMEM;
		goto err_free_urb_out_buf;
	}

	pcu->urb_ctrl = usb_alloc_urb(0, GFP_KERNEL);
	if (!pcu->urb_ctrl) {
		dev_err(pcu->dev, "Failed to allocate input URB\n");
		error = -ENOMEM;
		goto err_free_urb_ctrl_buf;
	}

	pcu->urb_ctrl->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
	pcu->urb_ctrl->transfer_dma = pcu->ctrl_dma;

	usb_fill_int_urb(pcu->urb_ctrl, pcu->udev,
			  usb_rcvintpipe(pcu->udev,
					 pcu->ep_ctrl->bEndpointAddress),
			  pcu->urb_ctrl_buf, pcu->max_ctrl_size,
			  ims_pcu_irq, pcu, pcu->ep_ctrl->bInterval);

	return 0;

err_free_urb_ctrl_buf:
	usb_free_coherent(pcu->udev, pcu->max_ctrl_size,
			  pcu->urb_ctrl_buf, pcu->ctrl_dma);
err_free_urb_out_buf:
	kfree(pcu->urb_out_buf);
err_free_in_urb:
	usb_free_urb(pcu->urb_in);
err_free_urb_in_buf:
	usb_free_coherent(pcu->udev, pcu->max_in_size,
			  pcu->urb_in_buf, pcu->read_dma);
	return error;
}
