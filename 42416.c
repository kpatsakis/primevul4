static struct ip_vs_service *ip_vs_genl_find_service(struct net *net,
						     struct nlattr *nla)
{
	struct ip_vs_service_user_kern usvc;
	struct ip_vs_service *svc;
	int ret;

	ret = ip_vs_genl_parse_service(net, &usvc, nla, 0, &svc);
	return ret ? ERR_PTR(ret) : svc;
}
