static int cuse_parse_devinfo(char *p, size_t len, struct cuse_devinfo *devinfo)
{
	char *end = p + len;
	char *uninitialized_var(key), *uninitialized_var(val);
	int rc;

	while (true) {
		rc = cuse_parse_one(&p, end, &key, &val);
		if (rc < 0)
			return rc;
		if (!rc)
			break;
		if (strcmp(key, "DEVNAME") == 0)
			devinfo->name = val;
		else
			printk(KERN_WARNING "CUSE: unknown device info \"%s\"\n",
			       key);
	}

	if (!devinfo->name || !strlen(devinfo->name)) {
		printk(KERN_ERR "CUSE: DEVNAME unspecified\n");
		return -EINVAL;
	}

	return 0;
}
