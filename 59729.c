cifs_get_spnego_key(struct cifs_ses *sesInfo)
{
	struct TCP_Server_Info *server = sesInfo->server;
	struct sockaddr_in *sa = (struct sockaddr_in *) &server->dstaddr;
	struct sockaddr_in6 *sa6 = (struct sockaddr_in6 *) &server->dstaddr;
	char *description, *dp;
	size_t desc_len;
	struct key *spnego_key;
	const char *hostname = server->hostname;

	/* length of fields (with semicolons): ver=0xyz ip4=ipaddress
	   host=hostname sec=mechanism uid=0xFF user=username */
	desc_len = MAX_VER_STR_LEN +
		   HOST_KEY_LEN + strlen(hostname) +
		   IP_KEY_LEN + INET6_ADDRSTRLEN +
		   MAX_MECH_STR_LEN +
		   UID_KEY_LEN + (sizeof(uid_t) * 2) +
		   CREDUID_KEY_LEN + (sizeof(uid_t) * 2) +
		   PID_KEY_LEN + (sizeof(pid_t) * 2) + 1;

	if (sesInfo->user_name)
		desc_len += USER_KEY_LEN + strlen(sesInfo->user_name);

	spnego_key = ERR_PTR(-ENOMEM);
	description = kzalloc(desc_len, GFP_KERNEL);
	if (description == NULL)
		goto out;

	dp = description;
	/* start with version and hostname portion of UNC string */
	spnego_key = ERR_PTR(-EINVAL);
	sprintf(dp, "ver=0x%x;host=%s;", CIFS_SPNEGO_UPCALL_VERSION,
		hostname);
	dp = description + strlen(description);

	/* add the server address */
	if (server->dstaddr.ss_family == AF_INET)
		sprintf(dp, "ip4=%pI4", &sa->sin_addr);
	else if (server->dstaddr.ss_family == AF_INET6)
		sprintf(dp, "ip6=%pI6", &sa6->sin6_addr);
	else
		goto out;

	dp = description + strlen(description);

	/* for now, only sec=krb5 and sec=mskrb5 are valid */
	if (server->sec_kerberos)
		sprintf(dp, ";sec=krb5");
	else if (server->sec_mskerberos)
		sprintf(dp, ";sec=mskrb5");
	else
		goto out;

	dp = description + strlen(description);
	sprintf(dp, ";uid=0x%x",
		from_kuid_munged(&init_user_ns, sesInfo->linux_uid));

	dp = description + strlen(description);
	sprintf(dp, ";creduid=0x%x",
		from_kuid_munged(&init_user_ns, sesInfo->cred_uid));

	if (sesInfo->user_name) {
		dp = description + strlen(description);
		sprintf(dp, ";user=%s", sesInfo->user_name);
	}

	dp = description + strlen(description);
	sprintf(dp, ";pid=0x%x", current->pid);

	cifs_dbg(FYI, "key description = %s\n", description);
	spnego_key = request_key(&cifs_spnego_key_type, description, "");

#ifdef CONFIG_CIFS_DEBUG2
	if (cifsFYI && !IS_ERR(spnego_key)) {
		struct cifs_spnego_msg *msg = spnego_key->payload.data;
		cifs_dump_mem("SPNEGO reply blob:", msg->data, min(1024U,
				msg->secblob_len + msg->sesskey_len));
	}
#endif /* CONFIG_CIFS_DEBUG2 */

out:
	kfree(description);
	return spnego_key;
}
