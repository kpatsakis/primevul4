static int snd_usb_audio_create(struct usb_interface *intf,
				struct usb_device *dev, int idx,
				const struct snd_usb_audio_quirk *quirk,
				unsigned int usb_id,
				struct snd_usb_audio **rchip)
{
	struct snd_card *card;
	struct snd_usb_audio *chip;
	int err, len;
	char component[14];
	static struct snd_device_ops ops = {
		.dev_free =	snd_usb_audio_dev_free,
	};

	*rchip = NULL;

	switch (snd_usb_get_speed(dev)) {
	case USB_SPEED_LOW:
	case USB_SPEED_FULL:
	case USB_SPEED_HIGH:
	case USB_SPEED_WIRELESS:
	case USB_SPEED_SUPER:
	case USB_SPEED_SUPER_PLUS:
		break;
	default:
		dev_err(&dev->dev, "unknown device speed %d\n", snd_usb_get_speed(dev));
		return -ENXIO;
	}

	err = snd_card_new(&intf->dev, index[idx], id[idx], THIS_MODULE,
			   0, &card);
	if (err < 0) {
		dev_err(&dev->dev, "cannot create card instance %d\n", idx);
		return err;
	}

	chip = kzalloc(sizeof(*chip), GFP_KERNEL);
	if (! chip) {
		snd_card_free(card);
		return -ENOMEM;
	}

	mutex_init(&chip->mutex);
	init_waitqueue_head(&chip->shutdown_wait);
	chip->index = idx;
	chip->dev = dev;
	chip->card = card;
	chip->setup = device_setup[idx];
	chip->autoclock = autoclock;
	atomic_set(&chip->active, 1); /* avoid autopm during probing */
	atomic_set(&chip->usage_count, 0);
	atomic_set(&chip->shutdown, 0);

	chip->usb_id = usb_id;
	INIT_LIST_HEAD(&chip->pcm_list);
	INIT_LIST_HEAD(&chip->ep_list);
	INIT_LIST_HEAD(&chip->midi_list);
	INIT_LIST_HEAD(&chip->mixer_list);

	if ((err = snd_device_new(card, SNDRV_DEV_LOWLEVEL, chip, &ops)) < 0) {
		snd_usb_audio_free(chip);
		snd_card_free(card);
		return err;
	}

	strcpy(card->driver, "USB-Audio");
	sprintf(component, "USB%04x:%04x",
		USB_ID_VENDOR(chip->usb_id), USB_ID_PRODUCT(chip->usb_id));
	snd_component_add(card, component);

	/* retrieve the device string as shortname */
	if (quirk && quirk->product_name && *quirk->product_name) {
		strlcpy(card->shortname, quirk->product_name, sizeof(card->shortname));
	} else {
		if (!dev->descriptor.iProduct ||
		    usb_string(dev, dev->descriptor.iProduct,
		    card->shortname, sizeof(card->shortname)) <= 0) {
			/* no name available from anywhere, so use ID */
			sprintf(card->shortname, "USB Device %#04x:%#04x",
				USB_ID_VENDOR(chip->usb_id),
				USB_ID_PRODUCT(chip->usb_id));
		}
	}
	strim(card->shortname);

	/* retrieve the vendor and device strings as longname */
	if (quirk && quirk->vendor_name && *quirk->vendor_name) {
		len = strlcpy(card->longname, quirk->vendor_name, sizeof(card->longname));
	} else {
		if (dev->descriptor.iManufacturer)
			len = usb_string(dev, dev->descriptor.iManufacturer,
					 card->longname, sizeof(card->longname));
		else
			len = 0;
		/* we don't really care if there isn't any vendor string */
	}
	if (len > 0) {
		strim(card->longname);
		if (*card->longname)
			strlcat(card->longname, " ", sizeof(card->longname));
	}

	strlcat(card->longname, card->shortname, sizeof(card->longname));

	len = strlcat(card->longname, " at ", sizeof(card->longname));

	if (len < sizeof(card->longname))
		usb_make_path(dev, card->longname + len, sizeof(card->longname) - len);

	switch (snd_usb_get_speed(dev)) {
	case USB_SPEED_LOW:
		strlcat(card->longname, ", low speed", sizeof(card->longname));
		break;
	case USB_SPEED_FULL:
		strlcat(card->longname, ", full speed", sizeof(card->longname));
		break;
	case USB_SPEED_HIGH:
		strlcat(card->longname, ", high speed", sizeof(card->longname));
		break;
	case USB_SPEED_SUPER:
		strlcat(card->longname, ", super speed", sizeof(card->longname));
		break;
	case USB_SPEED_SUPER_PLUS:
		strlcat(card->longname, ", super speed plus", sizeof(card->longname));
		break;
	default:
		break;
	}

	snd_usb_audio_create_proc(chip);

	*rchip = chip;
	return 0;
}
