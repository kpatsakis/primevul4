monitor_valid_hostbasedblob(u_char *data, u_int datalen, char *cuser,
    char *chost)
{
	Buffer b;
	char *p, *userstyle;
	u_int len;
	int fail = 0;

	buffer_init(&b);
	buffer_append(&b, data, datalen);

	p = buffer_get_string(&b, &len);
	if ((session_id2 == NULL) ||
	    (len != session_id2_len) ||
	    (timingsafe_bcmp(p, session_id2, session_id2_len) != 0))
		fail++;
	free(p);

	if (buffer_get_char(&b) != SSH2_MSG_USERAUTH_REQUEST)
		fail++;
	p = buffer_get_cstring(&b, NULL);
	xasprintf(&userstyle, "%s%s%s", authctxt->user,
	    authctxt->style ? ":" : "",
	    authctxt->style ? authctxt->style : "");
	if (strcmp(userstyle, p) != 0) {
		logit("wrong user name passed to monitor: expected %s != %.100s",
		    userstyle, p);
		fail++;
	}
	free(userstyle);
	free(p);
	buffer_skip_string(&b);	/* service */
	p = buffer_get_cstring(&b, NULL);
	if (strcmp(p, "hostbased") != 0)
		fail++;
	free(p);
	buffer_skip_string(&b);	/* pkalg */
	buffer_skip_string(&b);	/* pkblob */

	/* verify client host, strip trailing dot if necessary */
	p = buffer_get_string(&b, NULL);
	if (((len = strlen(p)) > 0) && p[len - 1] == '.')
		p[len - 1] = '\0';
	if (strcmp(p, chost) != 0)
		fail++;
	free(p);

	/* verify client user */
	p = buffer_get_string(&b, NULL);
	if (strcmp(p, cuser) != 0)
		fail++;
	free(p);

	if (buffer_len(&b) != 0)
		fail++;
	buffer_free(&b);
	return (fail == 0);
}
