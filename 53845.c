static void aiptek_irq(struct urb *urb)
{
	struct aiptek *aiptek = urb->context;
	unsigned char *data = aiptek->data;
	struct input_dev *inputdev = aiptek->inputdev;
	struct usb_interface *intf = aiptek->intf;
	int jitterable = 0;
	int retval, macro, x, y, z, left, right, middle, p, dv, tip, bs, pck;

	switch (urb->status) {
	case 0:
		/* Success */
		break;

	case -ECONNRESET:
	case -ENOENT:
	case -ESHUTDOWN:
		/* This urb is terminated, clean up */
		dev_dbg(&intf->dev, "%s - urb shutting down with status: %d\n",
			__func__, urb->status);
		return;

	default:
		dev_dbg(&intf->dev, "%s - nonzero urb status received: %d\n",
			__func__, urb->status);
		goto exit;
	}

	/* See if we are in a delay loop -- throw out report if true.
	 */
	if (aiptek->inDelay == 1 && time_after(aiptek->endDelay, jiffies)) {
		goto exit;
	}

	aiptek->inDelay = 0;
	aiptek->eventCount++;

	/* Report 1 delivers relative coordinates with either a stylus
	 * or the mouse. You do not know, however, which input
	 * tool generated the event.
	 */
	if (data[0] == 1) {
		if (aiptek->curSetting.coordinateMode ==
		    AIPTEK_COORDINATE_ABSOLUTE_MODE) {
			aiptek->diagnostic =
			    AIPTEK_DIAGNOSTIC_SENDING_RELATIVE_IN_ABSOLUTE;
		} else {
			x = (signed char) data[2];
			y = (signed char) data[3];

			/* jitterable keeps track of whether any button has been pressed.
			 * We're also using it to remap the physical mouse button mask
			 * to pseudo-settings. (We don't specifically care about it's
			 * value after moving/transposing mouse button bitmasks, except
			 * that a non-zero value indicates that one or more
			 * mouse button was pressed.)
			 */
			jitterable = data[1] & 0x07;

			left = (data[1] & aiptek->curSetting.mouseButtonLeft >> 2) != 0 ? 1 : 0;
			right = (data[1] & aiptek->curSetting.mouseButtonRight >> 2) != 0 ? 1 : 0;
			middle = (data[1] & aiptek->curSetting.mouseButtonMiddle >> 2) != 0 ? 1 : 0;

			input_report_key(inputdev, BTN_LEFT, left);
			input_report_key(inputdev, BTN_MIDDLE, middle);
			input_report_key(inputdev, BTN_RIGHT, right);

			input_report_abs(inputdev, ABS_MISC,
					 1 | AIPTEK_REPORT_TOOL_UNKNOWN);
			input_report_rel(inputdev, REL_X, x);
			input_report_rel(inputdev, REL_Y, y);

			/* Wheel support is in the form of a single-event
			 * firing.
			 */
			if (aiptek->curSetting.wheel != AIPTEK_WHEEL_DISABLE) {
				input_report_rel(inputdev, REL_WHEEL,
						 aiptek->curSetting.wheel);
				aiptek->curSetting.wheel = AIPTEK_WHEEL_DISABLE;
			}
			if (aiptek->lastMacro != -1) {
			        input_report_key(inputdev,
						 macroKeyEvents[aiptek->lastMacro], 0);
				aiptek->lastMacro = -1;
			}
			input_sync(inputdev);
		}
	}
	/* Report 2 is delivered only by the stylus, and delivers
	 * absolute coordinates.
	 */
	else if (data[0] == 2) {
		if (aiptek->curSetting.coordinateMode == AIPTEK_COORDINATE_RELATIVE_MODE) {
			aiptek->diagnostic = AIPTEK_DIAGNOSTIC_SENDING_ABSOLUTE_IN_RELATIVE;
		} else if (!AIPTEK_POINTER_ALLOW_STYLUS_MODE
			    (aiptek->curSetting.pointerMode)) {
				aiptek->diagnostic = AIPTEK_DIAGNOSTIC_TOOL_DISALLOWED;
		} else {
			x = get_unaligned_le16(data + 1);
			y = get_unaligned_le16(data + 3);
			z = get_unaligned_le16(data + 6);

			dv = (data[5] & 0x01) != 0 ? 1 : 0;
			p = (data[5] & 0x02) != 0 ? 1 : 0;
			tip = (data[5] & 0x04) != 0 ? 1 : 0;

			/* Use jitterable to re-arrange button masks
			 */
			jitterable = data[5] & 0x18;

			bs = (data[5] & aiptek->curSetting.stylusButtonLower) != 0 ? 1 : 0;
			pck = (data[5] & aiptek->curSetting.stylusButtonUpper) != 0 ? 1 : 0;

			/* dv indicates 'data valid' (e.g., the tablet is in sync
			 * and has delivered a "correct" report) We will ignore
			 * all 'bad' reports...
			 */
			if (dv != 0) {
				/* If the selected tool changed, reset the old
				 * tool key, and set the new one.
				 */
				if (aiptek->previousToolMode !=
				    aiptek->curSetting.toolMode) {
				        input_report_key(inputdev,
							 aiptek->previousToolMode, 0);
					input_report_key(inputdev,
							 aiptek->curSetting.toolMode,
							 1);
					aiptek->previousToolMode =
					          aiptek->curSetting.toolMode;
				}

				if (p != 0) {
					input_report_abs(inputdev, ABS_X, x);
					input_report_abs(inputdev, ABS_Y, y);
					input_report_abs(inputdev, ABS_PRESSURE, z);

					input_report_key(inputdev, BTN_TOUCH, tip);
					input_report_key(inputdev, BTN_STYLUS, bs);
					input_report_key(inputdev, BTN_STYLUS2, pck);

					if (aiptek->curSetting.xTilt !=
					    AIPTEK_TILT_DISABLE) {
						input_report_abs(inputdev,
								 ABS_TILT_X,
								 aiptek->curSetting.xTilt);
					}
					if (aiptek->curSetting.yTilt != AIPTEK_TILT_DISABLE) {
						input_report_abs(inputdev,
								 ABS_TILT_Y,
								 aiptek->curSetting.yTilt);
					}

					/* Wheel support is in the form of a single-event
					 * firing.
					 */
					if (aiptek->curSetting.wheel !=
					    AIPTEK_WHEEL_DISABLE) {
						input_report_abs(inputdev,
								 ABS_WHEEL,
								 aiptek->curSetting.wheel);
						aiptek->curSetting.wheel = AIPTEK_WHEEL_DISABLE;
					}
				}
				input_report_abs(inputdev, ABS_MISC, p | AIPTEK_REPORT_TOOL_STYLUS);
				if (aiptek->lastMacro != -1) {
			                input_report_key(inputdev,
							 macroKeyEvents[aiptek->lastMacro], 0);
					aiptek->lastMacro = -1;
				}
				input_sync(inputdev);
			}
		}
	}
	/* Report 3's come from the mouse in absolute mode.
	 */
	else if (data[0] == 3) {
		if (aiptek->curSetting.coordinateMode == AIPTEK_COORDINATE_RELATIVE_MODE) {
			aiptek->diagnostic = AIPTEK_DIAGNOSTIC_SENDING_ABSOLUTE_IN_RELATIVE;
		} else if (!AIPTEK_POINTER_ALLOW_MOUSE_MODE
			(aiptek->curSetting.pointerMode)) {
			aiptek->diagnostic = AIPTEK_DIAGNOSTIC_TOOL_DISALLOWED;
		} else {
			x = get_unaligned_le16(data + 1);
			y = get_unaligned_le16(data + 3);

			jitterable = data[5] & 0x1c;

			dv = (data[5] & 0x01) != 0 ? 1 : 0;
			p = (data[5] & 0x02) != 0 ? 1 : 0;
			left = (data[5] & aiptek->curSetting.mouseButtonLeft) != 0 ? 1 : 0;
			right = (data[5] & aiptek->curSetting.mouseButtonRight) != 0 ? 1 : 0;
			middle = (data[5] & aiptek->curSetting.mouseButtonMiddle) != 0 ? 1 : 0;

			if (dv != 0) {
				/* If the selected tool changed, reset the old
				 * tool key, and set the new one.
				 */
				if (aiptek->previousToolMode !=
				    aiptek->curSetting.toolMode) {
				        input_report_key(inputdev,
							 aiptek->previousToolMode, 0);
					input_report_key(inputdev,
							 aiptek->curSetting.toolMode,
							 1);
					aiptek->previousToolMode =
					          aiptek->curSetting.toolMode;
				}

				if (p != 0) {
					input_report_abs(inputdev, ABS_X, x);
					input_report_abs(inputdev, ABS_Y, y);

					input_report_key(inputdev, BTN_LEFT, left);
					input_report_key(inputdev, BTN_MIDDLE, middle);
					input_report_key(inputdev, BTN_RIGHT, right);

					/* Wheel support is in the form of a single-event
					 * firing.
					 */
					if (aiptek->curSetting.wheel != AIPTEK_WHEEL_DISABLE) {
						input_report_abs(inputdev,
								 ABS_WHEEL,
								 aiptek->curSetting.wheel);
						aiptek->curSetting.wheel = AIPTEK_WHEEL_DISABLE;
					}
				}
				input_report_abs(inputdev, ABS_MISC, p | AIPTEK_REPORT_TOOL_MOUSE);
				if (aiptek->lastMacro != -1) {
			                input_report_key(inputdev,
							 macroKeyEvents[aiptek->lastMacro], 0);
				        aiptek->lastMacro = -1;
				}
				input_sync(inputdev);
			}
		}
	}
	/* Report 4s come from the macro keys when pressed by stylus
	 */
	else if (data[0] == 4) {
		jitterable = data[1] & 0x18;

		dv = (data[1] & 0x01) != 0 ? 1 : 0;
		p = (data[1] & 0x02) != 0 ? 1 : 0;
		tip = (data[1] & 0x04) != 0 ? 1 : 0;
		bs = (data[1] & aiptek->curSetting.stylusButtonLower) != 0 ? 1 : 0;
		pck = (data[1] & aiptek->curSetting.stylusButtonUpper) != 0 ? 1 : 0;

		macro = dv && p && tip && !(data[3] & 1) ? (data[3] >> 1) : -1;
		z = get_unaligned_le16(data + 4);

		if (dv) {
		        /* If the selected tool changed, reset the old
			 * tool key, and set the new one.
			 */
		        if (aiptek->previousToolMode !=
			    aiptek->curSetting.toolMode) {
			        input_report_key(inputdev,
						 aiptek->previousToolMode, 0);
				input_report_key(inputdev,
						 aiptek->curSetting.toolMode,
						 1);
				aiptek->previousToolMode =
				        aiptek->curSetting.toolMode;
			}
		}

		if (aiptek->lastMacro != -1 && aiptek->lastMacro != macro) {
		        input_report_key(inputdev, macroKeyEvents[aiptek->lastMacro], 0);
			aiptek->lastMacro = -1;
		}

		if (macro != -1 && macro != aiptek->lastMacro) {
			input_report_key(inputdev, macroKeyEvents[macro], 1);
			aiptek->lastMacro = macro;
		}
		input_report_abs(inputdev, ABS_MISC,
				 p | AIPTEK_REPORT_TOOL_STYLUS);
		input_sync(inputdev);
	}
	/* Report 5s come from the macro keys when pressed by mouse
	 */
	else if (data[0] == 5) {
		jitterable = data[1] & 0x1c;

		dv = (data[1] & 0x01) != 0 ? 1 : 0;
		p = (data[1] & 0x02) != 0 ? 1 : 0;
		left = (data[1]& aiptek->curSetting.mouseButtonLeft) != 0 ? 1 : 0;
		right = (data[1] & aiptek->curSetting.mouseButtonRight) != 0 ? 1 : 0;
		middle = (data[1] & aiptek->curSetting.mouseButtonMiddle) != 0 ? 1 : 0;
		macro = dv && p && left && !(data[3] & 1) ? (data[3] >> 1) : 0;

		if (dv) {
		        /* If the selected tool changed, reset the old
			 * tool key, and set the new one.
			 */
		        if (aiptek->previousToolMode !=
			    aiptek->curSetting.toolMode) {
		                input_report_key(inputdev,
						 aiptek->previousToolMode, 0);
			        input_report_key(inputdev,
						 aiptek->curSetting.toolMode, 1);
			        aiptek->previousToolMode = aiptek->curSetting.toolMode;
			}
		}

		if (aiptek->lastMacro != -1 && aiptek->lastMacro != macro) {
		        input_report_key(inputdev, macroKeyEvents[aiptek->lastMacro], 0);
			aiptek->lastMacro = -1;
		}

		if (macro != -1 && macro != aiptek->lastMacro) {
			input_report_key(inputdev, macroKeyEvents[macro], 1);
			aiptek->lastMacro = macro;
		}

		input_report_abs(inputdev, ABS_MISC,
				 p | AIPTEK_REPORT_TOOL_MOUSE);
		input_sync(inputdev);
	}
	/* We have no idea which tool can generate a report 6. Theoretically,
	 * neither need to, having been given reports 4 & 5 for such use.
	 * However, report 6 is the 'official-looking' report for macroKeys;
	 * reports 4 & 5 supposively are used to support unnamed, unknown
	 * hat switches (which just so happen to be the macroKeys.)
	 */
	else if (data[0] == 6) {
		macro = get_unaligned_le16(data + 1);
		if (macro > 0) {
			input_report_key(inputdev, macroKeyEvents[macro - 1],
					 0);
		}
		if (macro < 25) {
			input_report_key(inputdev, macroKeyEvents[macro + 1],
					 0);
		}

		/* If the selected tool changed, reset the old
		   tool key, and set the new one.
		*/
		if (aiptek->previousToolMode !=
		    aiptek->curSetting.toolMode) {
		        input_report_key(inputdev,
					 aiptek->previousToolMode, 0);
			input_report_key(inputdev,
					 aiptek->curSetting.toolMode,
					 1);
			aiptek->previousToolMode =
				aiptek->curSetting.toolMode;
		}

		input_report_key(inputdev, macroKeyEvents[macro], 1);
		input_report_abs(inputdev, ABS_MISC,
				 1 | AIPTEK_REPORT_TOOL_UNKNOWN);
		input_sync(inputdev);
	} else {
		dev_dbg(&intf->dev, "Unknown report %d\n", data[0]);
	}

	/* Jitter may occur when the user presses a button on the stlyus
	 * or the mouse. What we do to prevent that is wait 'x' milliseconds
	 * following a 'jitterable' event, which should give the hand some time
	 * stabilize itself.
	 *
	 * We just introduced aiptek->previousJitterable to carry forth the
	 * notion that jitter occurs when the button state changes from on to off:
	 * a person drawing, holding a button down is not subject to jittering.
	 * With that in mind, changing from upper button depressed to lower button
	 * WILL transition through a jitter delay.
	 */

	if (aiptek->previousJitterable != jitterable &&
	    aiptek->curSetting.jitterDelay != 0 && aiptek->inDelay != 1) {
		aiptek->endDelay = jiffies +
		    ((aiptek->curSetting.jitterDelay * HZ) / 1000);
		aiptek->inDelay = 1;
	}
	aiptek->previousJitterable = jitterable;

exit:
	retval = usb_submit_urb(urb, GFP_ATOMIC);
	if (retval != 0) {
		dev_err(&intf->dev,
			"%s - usb_submit_urb failed with result %d\n",
			__func__, retval);
	}
}
