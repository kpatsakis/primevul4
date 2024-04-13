static char* tcmu_get_path( struct tcmu_device *dev)
{
	char *config;

	config = strchr(tcmu_get_dev_cfgstring(dev), '/');
	if (!config) {
		tcmu_err("no configuration found in cfgstring\n");
		return NULL;
	}
	config += 1; /* get past '/' */

 	return config;
 }
