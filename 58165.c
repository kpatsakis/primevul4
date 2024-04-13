static int dvb_usb_stop_feed(struct dvb_demux_feed *dvbdmxfeed)
{
	struct dvb_usb_adapter *adap = dvbdmxfeed->demux->priv;
	struct dvb_usb_device *d = adap_to_d(adap);
	int ret = 0;
	dev_dbg(&d->udev->dev,
			"%s: adap=%d active_fe=%d feed_type=%d setting pid [%s]: %04x (%04d) at index %d\n",
			__func__, adap->id, adap->active_fe, dvbdmxfeed->type,
			adap->pid_filtering ? "yes" : "no", dvbdmxfeed->pid,
			dvbdmxfeed->pid, dvbdmxfeed->index);

	if (adap->active_fe == -1)
		return -EINVAL;

	/* remove PID from device HW PID filter */
	if (adap->pid_filtering && adap->props->pid_filter) {
		ret = adap->props->pid_filter(adap, dvbdmxfeed->index,
				dvbdmxfeed->pid, 0);
		if (ret)
			dev_err(&d->udev->dev, "%s: pid_filter() failed=%d\n",
					KBUILD_MODNAME, ret);
	}

	/* we cannot stop streaming until last PID is removed */
	if (--adap->feed_count > 0)
		goto skip_feed_stop;

	/* ask device to stop streaming */
	if (d->props->streaming_ctrl) {
		ret = d->props->streaming_ctrl(adap->fe[adap->active_fe], 0);
		if (ret)
			dev_err(&d->udev->dev,
					"%s: streaming_ctrl() failed=%d\n",
					KBUILD_MODNAME, ret);
	}

	/* disable HW PID filter */
	if (adap->pid_filtering && adap->props->pid_filter_ctrl) {
		ret = adap->props->pid_filter_ctrl(adap, 0);
		if (ret)
			dev_err(&d->udev->dev,
					"%s: pid_filter_ctrl() failed=%d\n",
					KBUILD_MODNAME, ret);
	}

	/* kill USB streaming packets */
	usb_urb_killv2(&adap->stream);

	/* clear 'streaming' status bit */
	clear_bit(ADAP_STREAMING, &adap->state_bits);
	smp_mb__after_atomic();
	wake_up_bit(&adap->state_bits, ADAP_STREAMING);
skip_feed_stop:

	if (ret)
		dev_dbg(&d->udev->dev, "%s: failed=%d\n", __func__, ret);
	return ret;
}
