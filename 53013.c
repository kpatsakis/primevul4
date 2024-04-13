static void powermate_sync_state(struct powermate_device *pm)
{
	if (pm->requires_update == 0)
		return; /* no updates are required */
	if (pm->config->status == -EINPROGRESS)
		return; /* an update is already in progress; it'll issue this update when it completes */

	if (pm->requires_update & UPDATE_PULSE_ASLEEP){
		pm->configcr->wValue = cpu_to_le16( SET_PULSE_ASLEEP );
		pm->configcr->wIndex = cpu_to_le16( pm->pulse_asleep ? 1 : 0 );
		pm->requires_update &= ~UPDATE_PULSE_ASLEEP;
	}else if (pm->requires_update & UPDATE_PULSE_AWAKE){
		pm->configcr->wValue = cpu_to_le16( SET_PULSE_AWAKE );
		pm->configcr->wIndex = cpu_to_le16( pm->pulse_awake ? 1 : 0 );
		pm->requires_update &= ~UPDATE_PULSE_AWAKE;
	}else if (pm->requires_update & UPDATE_PULSE_MODE){
		int op, arg;
		/* the powermate takes an operation and an argument for its pulse algorithm.
		   the operation can be:
		   0: divide the speed
		   1: pulse at normal speed
		   2: multiply the speed
		   the argument only has an effect for operations 0 and 2, and ranges between
		   1 (least effect) to 255 (maximum effect).

		   thus, several states are equivalent and are coalesced into one state.

		   we map this onto a range from 0 to 510, with:
		   0 -- 254    -- use divide (0 = slowest)
		   255         -- use normal speed
		   256 -- 510  -- use multiple (510 = fastest).

		   Only values of 'arg' quite close to 255 are particularly useful/spectacular.
		*/
		if (pm->pulse_speed < 255) {
			op = 0;                   // divide
			arg = 255 - pm->pulse_speed;
		} else if (pm->pulse_speed > 255) {
			op = 2;                   // multiply
			arg = pm->pulse_speed - 255;
		} else {
			op = 1;                   // normal speed
			arg = 0;                  // can be any value
		}
		pm->configcr->wValue = cpu_to_le16( (pm->pulse_table << 8) | SET_PULSE_MODE );
		pm->configcr->wIndex = cpu_to_le16( (arg << 8) | op );
		pm->requires_update &= ~UPDATE_PULSE_MODE;
	} else if (pm->requires_update & UPDATE_STATIC_BRIGHTNESS) {
		pm->configcr->wValue = cpu_to_le16( SET_STATIC_BRIGHTNESS );
		pm->configcr->wIndex = cpu_to_le16( pm->static_brightness );
		pm->requires_update &= ~UPDATE_STATIC_BRIGHTNESS;
	} else {
		printk(KERN_ERR "powermate: unknown update required");
		pm->requires_update = 0; /* fudge the bug */
		return;
	}

/*	printk("powermate: %04x %04x\n", pm->configcr->wValue, pm->configcr->wIndex); */

	pm->configcr->bRequestType = 0x41; /* vendor request */
	pm->configcr->bRequest = 0x01;
	pm->configcr->wLength = 0;

	usb_fill_control_urb(pm->config, pm->udev, usb_sndctrlpipe(pm->udev, 0),
			     (void *) pm->configcr, NULL, 0,
			     powermate_config_complete, pm);

	if (usb_submit_urb(pm->config, GFP_ATOMIC))
		printk(KERN_ERR "powermate: usb_submit_urb(config) failed");
}
