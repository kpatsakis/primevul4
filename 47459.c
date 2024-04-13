void ndisc_late_cleanup(void)
{
	unregister_netdevice_notifier(&ndisc_netdev_notifier);
}
