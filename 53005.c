static void gtco_setup_caps(struct input_dev *inputdev)
{
	struct gtco *device = input_get_drvdata(inputdev);

	/* Which events */
	inputdev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS) |
		BIT_MASK(EV_MSC);

	/* Misc event menu block */
	inputdev->mscbit[0] = BIT_MASK(MSC_SCAN) | BIT_MASK(MSC_SERIAL) |
		BIT_MASK(MSC_RAW);

	/* Absolute values based on HID report info */
	input_set_abs_params(inputdev, ABS_X, device->min_X, device->max_X,
			     0, 0);
	input_set_abs_params(inputdev, ABS_Y, device->min_Y, device->max_Y,
			     0, 0);

	/* Proximity */
	input_set_abs_params(inputdev, ABS_DISTANCE, 0, 1, 0, 0);

	/* Tilt & pressure */
	input_set_abs_params(inputdev, ABS_TILT_X, device->mintilt_X,
			     device->maxtilt_X, 0, 0);
	input_set_abs_params(inputdev, ABS_TILT_Y, device->mintilt_Y,
			     device->maxtilt_Y, 0, 0);
	input_set_abs_params(inputdev, ABS_PRESSURE, device->minpressure,
			     device->maxpressure, 0, 0);

	/* Transducer */
	input_set_abs_params(inputdev, ABS_MISC, 0, 0xFF, 0, 0);
}
