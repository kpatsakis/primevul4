static void __exit logi_dj_exit(void)
{
	dbg_hid("Logitech-DJ:%s\n", __func__);

	hid_unregister_driver(&logi_djdevice_driver);
	hid_unregister_driver(&logi_djreceiver_driver);

}
