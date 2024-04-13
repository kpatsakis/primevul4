static void __exit cma_cleanup(void)
{
	ibnl_remove_client(RDMA_NL_RDMA_CM);
	ib_unregister_client(&cma_client);
	unregister_netdevice_notifier(&cma_nb);
	rdma_addr_unregister_client(&addr_client);
	ib_sa_unregister_client(&sa_client);
	destroy_workqueue(cma_wq);
	idr_destroy(&tcp_ps);
	idr_destroy(&udp_ps);
	idr_destroy(&ipoib_ps);
	idr_destroy(&ib_ps);
}
