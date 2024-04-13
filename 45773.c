static void vsock_init_tables(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(vsock_bind_table); i++)
		INIT_LIST_HEAD(&vsock_bind_table[i]);

	for (i = 0; i < ARRAY_SIZE(vsock_connected_table); i++)
		INIT_LIST_HEAD(&vsock_connected_table[i]);
}
