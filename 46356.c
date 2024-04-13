mm_answer_pwnamallow(int sock, Buffer *m)
{
	char *username;
	struct passwd *pwent;
	int allowed = 0;
	u_int i;

	debug3("%s", __func__);

	if (authctxt->attempt++ != 0)
		fatal("%s: multiple attempts for getpwnam", __func__);

	username = buffer_get_string(m, NULL);

	pwent = getpwnamallow(username);

	authctxt->user = xstrdup(username);
	setproctitle("%s [priv]", pwent ? username : "unknown");
	free(username);

	buffer_clear(m);

	if (pwent == NULL) {
		buffer_put_char(m, 0);
		authctxt->pw = fakepw();
		goto out;
	}

	allowed = 1;
	authctxt->pw = pwent;
	authctxt->valid = 1;

	buffer_put_char(m, 1);
	buffer_put_string(m, pwent, sizeof(struct passwd));
	buffer_put_cstring(m, pwent->pw_name);
	buffer_put_cstring(m, "*");
#ifdef HAVE_STRUCT_PASSWD_PW_GECOS
	buffer_put_cstring(m, pwent->pw_gecos);
#endif
#ifdef HAVE_STRUCT_PASSWD_PW_CLASS
	buffer_put_cstring(m, pwent->pw_class);
#endif
	buffer_put_cstring(m, pwent->pw_dir);
	buffer_put_cstring(m, pwent->pw_shell);

 out:
	buffer_put_string(m, &options, sizeof(options));

#define M_CP_STROPT(x) do { \
		if (options.x != NULL) \
			buffer_put_cstring(m, options.x); \
	} while (0)
#define M_CP_STRARRAYOPT(x, nx) do { \
		for (i = 0; i < options.nx; i++) \
			buffer_put_cstring(m, options.x[i]); \
	} while (0)
	/* See comment in servconf.h */
	COPY_MATCH_STRING_OPTS();
#undef M_CP_STROPT
#undef M_CP_STRARRAYOPT

	/* Create valid auth method lists */
	if (compat20 && auth2_setup_methods_lists(authctxt) != 0) {
		/*
		 * The monitor will continue long enough to let the child
		 * run to it's packet_disconnect(), but it must not allow any
		 * authentication to succeed.
		 */
		debug("%s: no valid authentication method lists", __func__);
	}

	debug3("%s: sending MONITOR_ANS_PWNAM: %d", __func__, allowed);
	mm_request_send(sock, MONITOR_ANS_PWNAM, m);

	/* For SSHv1 allow authentication now */
	if (!compat20)
		monitor_permit_authentications(1);
	else {
		/* Allow service/style information on the auth context */
		monitor_permit(mon_dispatch, MONITOR_REQ_AUTHSERV, 1);
		monitor_permit(mon_dispatch, MONITOR_REQ_AUTH2_READ_BANNER, 1);
	}
#ifdef USE_PAM
	if (options.use_pam)
		monitor_permit(mon_dispatch, MONITOR_REQ_PAM_START, 1);
#endif

	return (0);
}
