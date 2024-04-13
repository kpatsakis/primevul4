static ssize_t ap_domain_show(struct bus_type *bus, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%d\n", ap_domain_index);
}
