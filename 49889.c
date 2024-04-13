static int __init ux500_hash_mod_init(void)
{
	klist_init(&driver_data.device_list, NULL, NULL);
	/* Initialize the semaphore to 0 devices (locked state) */
	sema_init(&driver_data.device_allocation, 0);

	return platform_driver_register(&hash_driver);
}
