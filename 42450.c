static void update_defense_level(struct netns_ipvs *ipvs)
{
	struct sysinfo i;
	static int old_secure_tcp = 0;
	int availmem;
	int nomem;
	int to_change = -1;

	/* we only count free and buffered memory (in pages) */
	si_meminfo(&i);
	availmem = i.freeram + i.bufferram;
	/* however in linux 2.5 the i.bufferram is total page cache size,
	   we need adjust it */
	/* si_swapinfo(&i); */
	/* availmem = availmem - (i.totalswap - i.freeswap); */

	nomem = (availmem < ipvs->sysctl_amemthresh);

	local_bh_disable();

	/* drop_entry */
	spin_lock(&ipvs->dropentry_lock);
	switch (ipvs->sysctl_drop_entry) {
	case 0:
		atomic_set(&ipvs->dropentry, 0);
		break;
	case 1:
		if (nomem) {
			atomic_set(&ipvs->dropentry, 1);
			ipvs->sysctl_drop_entry = 2;
		} else {
			atomic_set(&ipvs->dropentry, 0);
		}
		break;
	case 2:
		if (nomem) {
			atomic_set(&ipvs->dropentry, 1);
		} else {
			atomic_set(&ipvs->dropentry, 0);
			ipvs->sysctl_drop_entry = 1;
		};
		break;
	case 3:
		atomic_set(&ipvs->dropentry, 1);
		break;
	}
	spin_unlock(&ipvs->dropentry_lock);

	/* drop_packet */
	spin_lock(&ipvs->droppacket_lock);
	switch (ipvs->sysctl_drop_packet) {
	case 0:
		ipvs->drop_rate = 0;
		break;
	case 1:
		if (nomem) {
			ipvs->drop_rate = ipvs->drop_counter
				= ipvs->sysctl_amemthresh /
				(ipvs->sysctl_amemthresh-availmem);
			ipvs->sysctl_drop_packet = 2;
		} else {
			ipvs->drop_rate = 0;
		}
		break;
	case 2:
		if (nomem) {
			ipvs->drop_rate = ipvs->drop_counter
				= ipvs->sysctl_amemthresh /
				(ipvs->sysctl_amemthresh-availmem);
		} else {
			ipvs->drop_rate = 0;
			ipvs->sysctl_drop_packet = 1;
		}
		break;
	case 3:
		ipvs->drop_rate = ipvs->sysctl_am_droprate;
		break;
	}
	spin_unlock(&ipvs->droppacket_lock);

	/* secure_tcp */
	spin_lock(&ipvs->securetcp_lock);
	switch (ipvs->sysctl_secure_tcp) {
	case 0:
		if (old_secure_tcp >= 2)
			to_change = 0;
		break;
	case 1:
		if (nomem) {
			if (old_secure_tcp < 2)
				to_change = 1;
			ipvs->sysctl_secure_tcp = 2;
		} else {
			if (old_secure_tcp >= 2)
				to_change = 0;
		}
		break;
	case 2:
		if (nomem) {
			if (old_secure_tcp < 2)
				to_change = 1;
		} else {
			if (old_secure_tcp >= 2)
				to_change = 0;
			ipvs->sysctl_secure_tcp = 1;
		}
		break;
	case 3:
		if (old_secure_tcp < 2)
			to_change = 1;
		break;
	}
	old_secure_tcp = ipvs->sysctl_secure_tcp;
	if (to_change >= 0)
		ip_vs_protocol_timeout_change(ipvs,
					      ipvs->sysctl_secure_tcp > 1);
	spin_unlock(&ipvs->securetcp_lock);

	local_bh_enable();
}
