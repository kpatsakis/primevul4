static int __init ux500_cryp_mod_init(void)
{
	pr_debug("[%s] is called!", __func__);
	klist_init(&driver_data.device_list, NULL, NULL);
	/* Initialize the semaphore to 0 devices (locked state) */
	sema_init(&driver_data.device_allocation, 0);
	return platform_driver_register(&cryp_driver);
}
