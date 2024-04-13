int __init rd_module_init(void)
{
	int ret;

	ret = transport_subsystem_register(&rd_mcp_template);
	if (ret < 0) {
		return ret;
	}

	return 0;
}
