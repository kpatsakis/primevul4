static int __init logi_dj_init(void)
{
	int retval;

	dbg_hid("Logitech-DJ:%s\n", __func__);

	retval = hid_register_driver(&logi_djreceiver_driver);
	if (retval)
		return retval;

	retval = hid_register_driver(&logi_djdevice_driver);
	if (retval)
		hid_unregister_driver(&logi_djreceiver_driver);

	return retval;

}
