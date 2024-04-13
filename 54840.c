static void usb_audio_disconnect(struct usb_interface *intf)
{
	struct snd_usb_audio *chip = usb_get_intfdata(intf);
	struct snd_card *card;
	struct list_head *p;

	if (chip == (void *)-1L)
		return;

	card = chip->card;

	mutex_lock(&register_mutex);
	if (atomic_inc_return(&chip->shutdown) == 1) {
		struct snd_usb_stream *as;
		struct snd_usb_endpoint *ep;
		struct usb_mixer_interface *mixer;

		/* wait until all pending tasks done;
		 * they are protected by snd_usb_lock_shutdown()
		 */
		wait_event(chip->shutdown_wait,
			   !atomic_read(&chip->usage_count));
		snd_card_disconnect(card);
		/* release the pcm resources */
		list_for_each_entry(as, &chip->pcm_list, list) {
			snd_usb_stream_disconnect(as);
		}
		/* release the endpoint resources */
		list_for_each_entry(ep, &chip->ep_list, list) {
			snd_usb_endpoint_release(ep);
		}
		/* release the midi resources */
		list_for_each(p, &chip->midi_list) {
			snd_usbmidi_disconnect(p);
		}
		/* release mixer resources */
		list_for_each_entry(mixer, &chip->mixer_list, list) {
			snd_usb_mixer_disconnect(mixer);
		}
	}

	chip->num_interfaces--;
	if (chip->num_interfaces <= 0) {
		usb_chip[chip->index] = NULL;
		mutex_unlock(&register_mutex);
		snd_card_free_when_closed(card);
	} else {
		mutex_unlock(&register_mutex);
	}
}
