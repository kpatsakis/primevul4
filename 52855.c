static void snd_usbmidi_output_drain(struct snd_rawmidi_substream *substream)
{
	struct usbmidi_out_port *port = substream->runtime->private_data;
	struct snd_usb_midi_out_endpoint *ep = port->ep;
	unsigned int drain_urbs;
	DEFINE_WAIT(wait);
	long timeout = msecs_to_jiffies(50);

	if (ep->umidi->disconnected)
		return;
	/*
	 * The substream buffer is empty, but some data might still be in the
	 * currently active URBs, so we have to wait for those to complete.
	 */
	spin_lock_irq(&ep->buffer_lock);
	drain_urbs = ep->active_urbs;
	if (drain_urbs) {
		ep->drain_urbs |= drain_urbs;
		do {
			prepare_to_wait(&ep->drain_wait, &wait,
					TASK_UNINTERRUPTIBLE);
			spin_unlock_irq(&ep->buffer_lock);
			timeout = schedule_timeout(timeout);
			spin_lock_irq(&ep->buffer_lock);
			drain_urbs &= ep->drain_urbs;
		} while (drain_urbs && timeout);
		finish_wait(&ep->drain_wait, &wait);
	}
	spin_unlock_irq(&ep->buffer_lock);
}
