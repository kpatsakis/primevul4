static void picolcd_exit_keys(struct picolcd_data *data)
{
	struct input_dev *idev = data->input_keys;

	data->input_keys = NULL;
	if (idev)
		input_unregister_device(idev);
}
