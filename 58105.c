static void remove_port(struct kref *kref)
{
	struct port *port;

	port = container_of(kref, struct port, kref);

	kfree(port);
}
