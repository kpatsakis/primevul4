static int cxusb_dualdig4_rev2_frontend_attach(struct dvb_usb_adapter *adap)
{
	struct dib0700_adapter_state *state = adap->priv;

	if (usb_set_interface(adap->dev->udev, 0, 1) < 0)
		err("set interface failed");

	cxusb_ctrl_msg(adap->dev, CMD_DIGITAL, NULL, 0, NULL, 0);

	cxusb_bluebird_gpio_pulse(adap->dev, 0x02, 1);

	if (!dvb_attach(dib7000p_attach, &state->dib7000p_ops))
		return -ENODEV;

	if (state->dib7000p_ops.i2c_enumeration(&adap->dev->i2c_adap, 1, 18,
				       &cxusb_dualdig4_rev2_config) < 0) {
		printk(KERN_WARNING "Unable to enumerate dib7000p\n");
		return -ENODEV;
	}

	adap->fe_adap[0].fe = state->dib7000p_ops.init(&adap->dev->i2c_adap, 0x80,
					      &cxusb_dualdig4_rev2_config);
	if (adap->fe_adap[0].fe == NULL)
		return -EIO;

	return 0;
}
