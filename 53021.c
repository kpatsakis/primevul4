static int ati_remote2_open(struct input_dev *idev)
{
	struct ati_remote2 *ar2 = input_get_drvdata(idev);
	int r;

	dev_dbg(&ar2->intf[0]->dev, "%s()\n", __func__);

	r = usb_autopm_get_interface(ar2->intf[0]);
	if (r) {
		dev_err(&ar2->intf[0]->dev,
			"%s(): usb_autopm_get_interface() = %d\n", __func__, r);
		goto fail1;
	}

	mutex_lock(&ati_remote2_mutex);

	if (!(ar2->flags & ATI_REMOTE2_SUSPENDED)) {
		r = ati_remote2_submit_urbs(ar2);
		if (r)
			goto fail2;
	}

	ar2->flags |= ATI_REMOTE2_OPENED;

	mutex_unlock(&ati_remote2_mutex);

	usb_autopm_put_interface(ar2->intf[0]);

	return 0;

 fail2:
	mutex_unlock(&ati_remote2_mutex);
	usb_autopm_put_interface(ar2->intf[0]);
 fail1:
	return r;
}
