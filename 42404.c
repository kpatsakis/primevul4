static int ip_vs_del_service(struct ip_vs_service *svc)
{
	if (svc == NULL)
		return -EEXIST;
	ip_vs_unlink_service(svc);

	return 0;
}
