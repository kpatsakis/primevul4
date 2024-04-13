static ssize_t ap_interrupts_show(struct bus_type *bus, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%d\n",
			ap_using_interrupts() ? 1 : 0);
}
