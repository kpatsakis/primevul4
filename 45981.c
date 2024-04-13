setup_deviceinfo(struct Interface *iface)
{
	struct ifreq	ifr;
	struct AdvPrefix *prefix;
	char zero[sizeof(iface->if_addr)];

	strncpy(ifr.ifr_name, iface->Name, IFNAMSIZ-1);
	ifr.ifr_name[IFNAMSIZ-1] = '\0';

	if (ioctl(sock, SIOCGIFMTU, &ifr) < 0) {
		flog(LOG_ERR, "ioctl(SIOCGIFMTU) failed for %s: %s",
			iface->Name, strerror(errno));
		return (-1);
	}

	dlog(LOG_DEBUG, 3, "mtu for %s is %d", iface->Name, ifr.ifr_mtu);
	iface->if_maxmtu = ifr.ifr_mtu;

	if (ioctl(sock, SIOCGIFHWADDR, &ifr) < 0)
	{
		flog(LOG_ERR, "ioctl(SIOCGIFHWADDR) failed for %s: %s",
			iface->Name, strerror(errno));
		return (-1);
	}

	dlog(LOG_DEBUG, 3, "hardware type for %s is %d", iface->Name,
		ifr.ifr_hwaddr.sa_family);

	switch(ifr.ifr_hwaddr.sa_family)
        {
	case ARPHRD_ETHER:
		iface->if_hwaddr_len = 48;
		iface->if_prefix_len = 64;
		break;
#ifdef ARPHRD_FDDI
	case ARPHRD_FDDI:
		iface->if_hwaddr_len = 48;
		iface->if_prefix_len = 64;
		break;
#endif /* ARPHDR_FDDI */
#ifdef ARPHRD_ARCNET
	case ARPHRD_ARCNET:
		iface->if_hwaddr_len = 8;
		iface->if_prefix_len = -1;
		iface->if_maxmtu = -1;
		break;
#endif /* ARPHDR_ARCNET */
	default:
		iface->if_hwaddr_len = -1;
		iface->if_prefix_len = -1;
		iface->if_maxmtu = -1;
		break;
	}

	dlog(LOG_DEBUG, 3, "link layer token length for %s is %d", iface->Name,
		iface->if_hwaddr_len);

	dlog(LOG_DEBUG, 3, "prefix length for %s is %d", iface->Name,
		iface->if_prefix_len);

	if (iface->if_hwaddr_len != -1) {
		unsigned int if_hwaddr_len_bytes = (iface->if_hwaddr_len + 7) >> 3;

		if (if_hwaddr_len_bytes > sizeof(iface->if_hwaddr)) {
			flog(LOG_ERR, "address length %d too big for %s", if_hwaddr_len_bytes, iface->Name);
			return(-2);
		}
		memcpy(iface->if_hwaddr, ifr.ifr_hwaddr.sa_data, if_hwaddr_len_bytes);

		memset(zero, 0, sizeof(zero));
		if (!memcmp(iface->if_hwaddr, zero, if_hwaddr_len_bytes))
			flog(LOG_WARNING, "WARNING, MAC address on %s is all zero!",
				iface->Name);
	}

	prefix = iface->AdvPrefixList;
	while (prefix)
	{
		if ((iface->if_prefix_len != -1) &&
		   (iface->if_prefix_len != prefix->PrefixLen))
		{
			flog(LOG_WARNING, "prefix length should be %d for %s",
				iface->if_prefix_len, iface->Name);
 		}

 		prefix = prefix->next;
	}

	return (0);
}
