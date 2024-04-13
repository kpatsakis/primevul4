static void x25_destroy_timer(unsigned long data)
{
	x25_destroy_socket_from_timer((struct sock *)data);
}
