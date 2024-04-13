void rd_module_exit(void)
{
	transport_subsystem_release(&rd_mcp_template);
}
