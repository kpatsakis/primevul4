int __init rawv6_init(void)
{
	int ret;

	ret = inet6_register_protosw(&rawv6_protosw);
	if (ret)
		goto out;
out:
	return ret;
}
