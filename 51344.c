void sigterm_handler() {
	struct mt_connection *p;
	struct mt_packet pdata;
	struct net_interface *interface, *tmp;
	/*_ Please include both \r and \n in translation, this is needed for the terminal emulator. */
	char message[] = gettext_noop("\r\n\r\nDaemon shutting down.\r\n");

	syslog(LOG_NOTICE, _("Daemon shutting down"));

	DL_FOREACH(connections_head, p) {
		if (p->state == STATE_ACTIVE) {
			init_packet(&pdata, MT_PTYPE_DATA, p->interface->mac_addr, p->srcmac, p->seskey, p->outcounter);
			add_control_packet(&pdata, MT_CPTYPE_PLAINDATA, _(message), strlen(_(message)));
			send_udp(p, &pdata);

			init_packet(&pdata, MT_PTYPE_END, p->interface->mac_addr, p->srcmac, p->seskey, p->outcounter);
			send_udp(p, &pdata);
		}
	}

	/* Doesn't hurt to tidy up */
	close(sockfd);
	close(insockfd);
	if (!use_raw_socket) {
		DL_FOREACH(interfaces, interface) {
			if (interface->socketfd > 0)
				close(interface->socketfd);
		}
	}
	DL_FOREACH_SAFE(interfaces, interface, tmp) {
		DL_DELETE(interfaces, interface);
		free(interface);
	}
	closelog();
	exit(0);
}
