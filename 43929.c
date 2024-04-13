static int picolcd_probe_bootloader(struct hid_device *hdev, struct picolcd_data *data)
{
	picolcd_init_devfs(data, NULL, NULL,
			picolcd_out_report(REPORT_BL_READ_MEMORY, hdev),
			picolcd_out_report(REPORT_BL_WRITE_MEMORY, hdev), NULL);
	return 0;
}
