static int usb_req_set_sel(struct usb_device *udev, enum usb3_link_state state)
{
	struct usb_set_sel_req *sel_values;
	unsigned long long u1_sel;
	unsigned long long u1_pel;
	unsigned long long u2_sel;
	unsigned long long u2_pel;
	int ret;

	if (udev->state != USB_STATE_CONFIGURED)
		return 0;

	/* Convert SEL and PEL stored in ns to us */
	u1_sel = DIV_ROUND_UP(udev->u1_params.sel, 1000);
	u1_pel = DIV_ROUND_UP(udev->u1_params.pel, 1000);
	u2_sel = DIV_ROUND_UP(udev->u2_params.sel, 1000);
	u2_pel = DIV_ROUND_UP(udev->u2_params.pel, 1000);

	/*
	 * Make sure that the calculated SEL and PEL values for the link
	 * state we're enabling aren't bigger than the max SEL/PEL
	 * value that will fit in the SET SEL control transfer.
	 * Otherwise the device would get an incorrect idea of the exit
	 * latency for the link state, and could start a device-initiated
	 * U1/U2 when the exit latencies are too high.
	 */
	if ((state == USB3_LPM_U1 &&
				(u1_sel > USB3_LPM_MAX_U1_SEL_PEL ||
				 u1_pel > USB3_LPM_MAX_U1_SEL_PEL)) ||
			(state == USB3_LPM_U2 &&
			 (u2_sel > USB3_LPM_MAX_U2_SEL_PEL ||
			  u2_pel > USB3_LPM_MAX_U2_SEL_PEL))) {
		dev_dbg(&udev->dev, "Device-initiated %s disabled due to long SEL %llu us or PEL %llu us\n",
				usb3_lpm_names[state], u1_sel, u1_pel);
		return -EINVAL;
	}

	/*
	 * If we're enabling device-initiated LPM for one link state,
	 * but the other link state has a too high SEL or PEL value,
	 * just set those values to the max in the Set SEL request.
	 */
	if (u1_sel > USB3_LPM_MAX_U1_SEL_PEL)
		u1_sel = USB3_LPM_MAX_U1_SEL_PEL;

	if (u1_pel > USB3_LPM_MAX_U1_SEL_PEL)
		u1_pel = USB3_LPM_MAX_U1_SEL_PEL;

	if (u2_sel > USB3_LPM_MAX_U2_SEL_PEL)
		u2_sel = USB3_LPM_MAX_U2_SEL_PEL;

	if (u2_pel > USB3_LPM_MAX_U2_SEL_PEL)
		u2_pel = USB3_LPM_MAX_U2_SEL_PEL;

	/*
	 * usb_enable_lpm() can be called as part of a failed device reset,
	 * which may be initiated by an error path of a mass storage driver.
	 * Therefore, use GFP_NOIO.
	 */
	sel_values = kmalloc(sizeof *(sel_values), GFP_NOIO);
	if (!sel_values)
		return -ENOMEM;

	sel_values->u1_sel = u1_sel;
	sel_values->u1_pel = u1_pel;
	sel_values->u2_sel = cpu_to_le16(u2_sel);
	sel_values->u2_pel = cpu_to_le16(u2_pel);

	ret = usb_control_msg(udev, usb_sndctrlpipe(udev, 0),
			USB_REQ_SET_SEL,
			USB_RECIP_DEVICE,
			0, 0,
			sel_values, sizeof *(sel_values),
			USB_CTRL_SET_TIMEOUT);
	kfree(sel_values);
	return ret;
}
