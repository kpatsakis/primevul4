static bool create_nic(char *nic, char *br, int pid, char **cnic)
{
	char *veth1buf, *veth2buf;
	veth1buf = alloca(IFNAMSIZ);
	veth2buf = alloca(IFNAMSIZ);
	int ret, mtu;

	ret = snprintf(veth1buf, IFNAMSIZ, "%s", nic);
	if (ret < 0 || ret >= IFNAMSIZ) {
		fprintf(stderr, "host nic name too long\n");
		return false;
	}

	/* create the nics */
	if (instantiate_veth(veth1buf, &veth2buf) < 0) {
		fprintf(stderr, "Error creating veth tunnel\n");
		return false;
	}

	if (strcmp(br, "none") != 0) {
		/* copy the bridge's mtu to both ends */
		mtu = get_mtu(br);
		if (mtu != -1) {
			if (lxc_netdev_set_mtu(veth1buf, mtu) < 0 ||
					lxc_netdev_set_mtu(veth2buf, mtu) < 0) {
				fprintf(stderr, "Failed setting mtu\n");
				goto out_del;
			}
		}

		/* attach veth1 to bridge */
		if (lxc_bridge_attach(lxcpath, lxcname, br, veth1buf) < 0) {
			fprintf(stderr, "Error attaching %s to %s\n", veth1buf, br);
			goto out_del;
		}
	}

	/* pass veth2 to target netns */
	ret = lxc_netdev_move_by_name(veth2buf, pid, NULL);
	if (ret < 0) {
		fprintf(stderr, "Error moving %s to netns %d\n", veth2buf, pid);
		goto out_del;
	}
	*cnic = strdup(veth2buf);
	return true;

out_del:
	lxc_netdev_delete_by_name(veth1buf);
	return false;
}
