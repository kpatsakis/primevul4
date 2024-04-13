int authorized_client(CLIENT *opts) {
	const char *ERRMSG="Invalid entry '%s' in authfile '%s', so, refusing all connections.";
	FILE *f ;
	char line[LINELEN]; 
	char *tmp;
	struct in_addr addr;
	struct in_addr client;
	struct in_addr cltemp;
	int len;

	if ((f=fopen(opts->server->authname,"r"))==NULL) {
                msg(LOG_INFO, "Can't open authorization file %s (%s).",
                    opts->server->authname, strerror(errno));
		return 1 ; 
	}
  
  	inet_aton(opts->clientname, &client);
	while (fgets(line,LINELEN,f)!=NULL) {
		if((tmp=strchr(line, '/'))) {
			if(strlen(line)<=tmp-line) {
				msg(LOG_CRIT, ERRMSG, line, opts->server->authname);
				return 0;
			}
			*(tmp++)=0;
			if(!inet_aton(line,&addr)) {
				msg(LOG_CRIT, ERRMSG, line, opts->server->authname);
				return 0;
			}
			len=strtol(tmp, NULL, 0);
			addr.s_addr>>=32-len;
			addr.s_addr<<=32-len;
			memcpy(&cltemp,&client,sizeof(client));
			cltemp.s_addr>>=32-len;
			cltemp.s_addr<<=32-len;
			if(addr.s_addr == cltemp.s_addr) {
				return 1;
			}
		}
		if (strncmp(line,opts->clientname,strlen(opts->clientname))==0) {
			fclose(f);
			return 1;
		}
	}
	fclose(f);
	return 0;
}
