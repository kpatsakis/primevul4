static int ap_uevent (struct device *dev, struct kobj_uevent_env *env)
{
	struct ap_device *ap_dev = to_ap_dev(dev);
	int retval = 0;

	if (!ap_dev)
		return -ENODEV;

	/* Set up DEV_TYPE environment variable. */
	retval = add_uevent_var(env, "DEV_TYPE=%04X", ap_dev->device_type);
	if (retval)
		return retval;

	/* Add MODALIAS= */
	retval = add_uevent_var(env, "MODALIAS=ap:t%02X", ap_dev->device_type);

	return retval;
}
