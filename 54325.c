static ssize_t show_cons_active(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct console *cs[16];
	int i = 0;
	struct console *c;
	ssize_t count = 0;

	console_lock();
	for_each_console(c) {
		if (!c->device)
			continue;
		if (!c->write)
			continue;
		if ((c->flags & CON_ENABLED) == 0)
			continue;
		cs[i++] = c;
		if (i >= ARRAY_SIZE(cs))
			break;
	}
	while (i--)
		count += sprintf(buf + count, "%s%d%c",
				 cs[i]->name, cs[i]->index, i ? ' ':'\n');
	console_unlock();

	return count;
}
