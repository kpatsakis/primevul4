static void lg_remove(struct hid_device *hdev)
{
	struct lg_drv_data *drv_data = hid_get_drvdata(hdev);
	if (drv_data->quirks & LG_FF4)
		lg4ff_deinit(hdev);

	hid_hw_stop(hdev);
	kfree(drv_data);
}
