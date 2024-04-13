static struct kobject *md_probe(dev_t dev, int *part, void *data)
{
	md_alloc(dev, NULL);
	return NULL;
}
