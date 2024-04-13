static void free_urb_and_buffer(struct snd_usb_midi *umidi, struct urb *urb,
				unsigned int buffer_length)
{
	usb_free_coherent(umidi->dev, buffer_length,
			  urb->transfer_buffer, urb->transfer_dma);
	usb_free_urb(urb);
}
