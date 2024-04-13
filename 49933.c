static ssize_t ap_poll_thread_show(struct bus_type *bus, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%d\n", ap_poll_kthread ? 1 : 0);
}
