int set_peername(int net, CLIENT *client) {
	struct sockaddr_storage addrin;
	struct sockaddr_storage netaddr;
	struct sockaddr_in  *netaddr4 = NULL;
	struct sockaddr_in6 *netaddr6 = NULL;
	socklen_t addrinlen = sizeof( addrin );
	struct addrinfo hints;
	struct addrinfo *ai = NULL;
	char peername[NI_MAXHOST];
	char netname[NI_MAXHOST];
	char *tmp = NULL;
	int i;
	int e;
	int shift;

	if (getpeername(net, (struct sockaddr *) &addrin, &addrinlen) < 0) {
		msg(LOG_INFO, "getpeername failed: %m");
		return -1;
	}

	if((e = getnameinfo((struct sockaddr *)&addrin, addrinlen,
			peername, sizeof (peername), NULL, 0, NI_NUMERICHOST))) {
		msg(LOG_INFO, "getnameinfo failed: %s", gai_strerror(e));
		return -1;
	}

	memset(&hints, '\0', sizeof (hints));
	hints.ai_flags = AI_ADDRCONFIG;
	e = getaddrinfo(peername, NULL, &hints, &ai);

	if(e != 0) {
		msg(LOG_INFO, "getaddrinfo failed: %s", gai_strerror(e));
		freeaddrinfo(ai);
		return -1;
	}

	switch(client->server->virtstyle) {
		case VIRT_NONE:
			msg(LOG_DEBUG, "virtualization is off");
			client->exportname=g_strdup(client->server->exportname);
			break;
		case VIRT_IPHASH:
			msg(LOG_DEBUG, "virtstyle iphash");
			for(i=0;i<strlen(peername);i++) {
				if(peername[i]=='.') {
					peername[i]='/';
				}
			}
		case VIRT_IPLIT:
			msg(LOG_DEBUG, "virststyle ipliteral");
			client->exportname=g_strdup_printf(client->server->exportname, peername);
			break;
		case VIRT_CIDR:
			msg(LOG_DEBUG, "virtstyle cidr %d", client->server->cidrlen);
			memcpy(&netaddr, &addrin, addrinlen);
			if(ai->ai_family == AF_INET) {
				netaddr4 = (struct sockaddr_in *)&netaddr;
				(netaddr4->sin_addr).s_addr>>=32-(client->server->cidrlen);
				(netaddr4->sin_addr).s_addr<<=32-(client->server->cidrlen);

				getnameinfo((struct sockaddr *) netaddr4, addrinlen,
							netname, sizeof (netname), NULL, 0, NI_NUMERICHOST);
				tmp=g_strdup_printf("%s/%s", netname, peername);
			}else if(ai->ai_family == AF_INET6) {
				netaddr6 = (struct sockaddr_in6 *)&netaddr;

				shift = 128-(client->server->cidrlen);
				i = 3;
				while(shift >= 8) {
					((netaddr6->sin6_addr).s6_addr[i])=0;
					shift-=8;
					i--;
				}
				(netaddr6->sin6_addr).s6_addr[i]>>=shift;
				(netaddr6->sin6_addr).s6_addr[i]<<=shift;

				getnameinfo((struct sockaddr *)netaddr6, addrinlen,
					    netname, sizeof(netname), NULL, 0, NI_NUMERICHOST);
				tmp=g_strdup_printf("%s/%s", netname, peername);
			}

			if(tmp != NULL)
			  client->exportname=g_strdup_printf(client->server->exportname, tmp);

			break;
	}

	freeaddrinfo(ai);
        msg(LOG_INFO, "connect from %s, assigned file is %s",
            peername, client->exportname);
	client->clientname=g_strdup(peername);
	return 0;
}
