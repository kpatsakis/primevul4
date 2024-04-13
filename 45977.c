int check_allrouters_membership(struct Interface *iface)
{
	#define ALL_ROUTERS_MCAST "ff020000000000000000000000000002"

	FILE *fp;
	unsigned int if_idx, allrouters_ok=0;
	char addr[32+1];
	char buffer[301] = {""}, *str;
	int ret=0;

	if ((fp = fopen(PATH_PROC_NET_IGMP6, "r")) == NULL)
	{
		flog(LOG_ERR, "can't open %s: %s", PATH_PROC_NET_IGMP6,
			strerror(errno));
		return (-1);
	}

	str = fgets(buffer, 300, fp);

	while (str && (ret = sscanf(str, "%u %*s %32[0-9A-Fa-f]", &if_idx, addr)) ) {
		if (ret == 2) {
			if (iface->if_index == if_idx) {
				if (strncmp(addr, ALL_ROUTERS_MCAST, sizeof(addr)) == 0){
					allrouters_ok = 1;
					break;
				}
			}
		}
		str = fgets(buffer, 300, fp);
	}

	fclose(fp);

	if (!allrouters_ok) {
		flog(LOG_WARNING, "resetting ipv6-allrouters membership on %s", iface->Name);
		setup_allrouters_membership(iface);
	}

	return(0);
}
