static int oz_get_port_from_addr(struct oz_hcd *ozhcd, u8 bus_addr)
{
	int i;

	for (i = 0; i < OZ_NB_PORTS; i++) {
		if (ozhcd->ports[i].bus_addr == bus_addr)
			return i;
	}
	return ozhcd->conn_port;
}
