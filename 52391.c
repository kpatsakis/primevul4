static void ims_pcu_buffers_free(struct ims_pcu *pcu)
{
	usb_kill_urb(pcu->urb_in);
	usb_free_urb(pcu->urb_in);

	usb_free_coherent(pcu->udev, pcu->max_out_size,
			  pcu->urb_in_buf, pcu->read_dma);

	kfree(pcu->urb_out_buf);

	usb_kill_urb(pcu->urb_ctrl);
	usb_free_urb(pcu->urb_ctrl);

	usb_free_coherent(pcu->udev, pcu->max_ctrl_size,
			  pcu->urb_ctrl_buf, pcu->ctrl_dma);
}
