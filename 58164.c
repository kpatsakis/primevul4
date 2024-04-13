static int dvb_usb_start_feed(struct dvb_demux_feed *dvbdmxfeed)
{
	struct dvb_usb_adapter *adap = dvbdmxfeed->demux->priv;
	struct dvb_usb_device *d = adap_to_d(adap);
	int ret = 0;
	struct usb_data_stream_properties stream_props;
	dev_dbg(&d->udev->dev,
			"%s: adap=%d active_fe=%d feed_type=%d setting pid [%s]: %04x (%04d) at index %d\n",
			__func__, adap->id, adap->active_fe, dvbdmxfeed->type,
			adap->pid_filtering ? "yes" : "no", dvbdmxfeed->pid,
			dvbdmxfeed->pid, dvbdmxfeed->index);

	/* wait init is done */
	wait_on_bit(&adap->state_bits, ADAP_INIT, TASK_UNINTERRUPTIBLE);

	if (adap->active_fe == -1)
		return -EINVAL;

	/* skip feed setup if we are already feeding */
	if (adap->feed_count++ > 0)
		goto skip_feed_start;

	/* set 'streaming' status bit */
	set_bit(ADAP_STREAMING, &adap->state_bits);

	/* resolve input and output streaming parameters */
	if (d->props->get_stream_config) {
		memcpy(&stream_props, &adap->props->stream,
				sizeof(struct usb_data_stream_properties));
		ret = d->props->get_stream_config(adap->fe[adap->active_fe],
				&adap->ts_type, &stream_props);
		if (ret)
			dev_err(&d->udev->dev,
					"%s: get_stream_config() failed=%d\n",
					KBUILD_MODNAME, ret);
	} else {
		stream_props = adap->props->stream;
	}

	switch (adap->ts_type) {
	case DVB_USB_FE_TS_TYPE_204:
		adap->stream.complete = dvb_usb_data_complete_204;
		break;
	case DVB_USB_FE_TS_TYPE_RAW:
		adap->stream.complete = dvb_usb_data_complete_raw;
		break;
	case DVB_USB_FE_TS_TYPE_188:
	default:
		adap->stream.complete = dvb_usb_data_complete;
		break;
	}

	/* submit USB streaming packets */
	usb_urb_submitv2(&adap->stream, &stream_props);

	/* enable HW PID filter */
	if (adap->pid_filtering && adap->props->pid_filter_ctrl) {
		ret = adap->props->pid_filter_ctrl(adap, 1);
		if (ret)
			dev_err(&d->udev->dev,
					"%s: pid_filter_ctrl() failed=%d\n",
					KBUILD_MODNAME, ret);
	}

	/* ask device to start streaming */
	if (d->props->streaming_ctrl) {
		ret = d->props->streaming_ctrl(adap->fe[adap->active_fe], 1);
		if (ret)
			dev_err(&d->udev->dev,
					"%s: streaming_ctrl() failed=%d\n",
					KBUILD_MODNAME, ret);
	}
skip_feed_start:

	/* add PID to device HW PID filter */
	if (adap->pid_filtering && adap->props->pid_filter) {
		ret = adap->props->pid_filter(adap, dvbdmxfeed->index,
				dvbdmxfeed->pid, 1);
		if (ret)
			dev_err(&d->udev->dev, "%s: pid_filter() failed=%d\n",
					KBUILD_MODNAME, ret);
	}

	if (ret)
		dev_dbg(&d->udev->dev, "%s: failed=%d\n", __func__, ret);
	return ret;
}
