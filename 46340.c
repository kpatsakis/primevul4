mm_answer_authserv(int sock, Buffer *m)
{
	monitor_permit_authentications(1);

	authctxt->service = buffer_get_string(m, NULL);
	authctxt->style = buffer_get_string(m, NULL);
	debug3("%s: service=%s, style=%s",
	    __func__, authctxt->service, authctxt->style);

	if (strlen(authctxt->style) == 0) {
		free(authctxt->style);
		authctxt->style = NULL;
	}

	return (0);
}
