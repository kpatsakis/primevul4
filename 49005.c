GArray* parse_cfile(gchar* f, struct generic_conf *const genconf, GError** e) {
	const char* DEFAULT_ERROR = "Could not parse %s in group %s: %s";
	const char* MISSING_REQUIRED_ERROR = "Could not find required value %s in group %s: %s";
	gchar* cfdir = NULL;
	SERVER s;
	gchar *virtstyle=NULL;
	PARAM lp[] = {
		{ "exportname", TRUE,	PARAM_STRING, 	&(s.exportname),	0 },
		{ "port", 	TRUE,	PARAM_INT, 	&(s.port),		0 },
		{ "authfile",	FALSE,	PARAM_STRING,	&(s.authname),		0 },
		{ "filesize",	FALSE,	PARAM_OFFT,	&(s.expected_size),	0 },
		{ "virtstyle",	FALSE,	PARAM_STRING,	&(virtstyle),		0 },
		{ "prerun",	FALSE,	PARAM_STRING,	&(s.prerun),		0 },
		{ "postrun",	FALSE,	PARAM_STRING,	&(s.postrun),		0 },
		{ "transactionlog", FALSE, PARAM_STRING, &(s.transactionlog),	0 },
		{ "readonly",	FALSE,	PARAM_BOOL,	&(s.flags),		F_READONLY },
		{ "multifile",	FALSE,	PARAM_BOOL,	&(s.flags),		F_MULTIFILE },
		{ "copyonwrite", FALSE,	PARAM_BOOL,	&(s.flags),		F_COPYONWRITE },
		{ "sparse_cow",	FALSE,	PARAM_BOOL,	&(s.flags),		F_SPARSE },
		{ "sdp",	FALSE,	PARAM_BOOL,	&(s.flags),		F_SDP },
		{ "sync",	FALSE,  PARAM_BOOL,	&(s.flags),		F_SYNC },
		{ "flush",	FALSE,  PARAM_BOOL,	&(s.flags),		F_FLUSH },
		{ "fua",	FALSE,  PARAM_BOOL,	&(s.flags),		F_FUA },
		{ "rotational",	FALSE,  PARAM_BOOL,	&(s.flags),		F_ROTATIONAL },
		{ "temporary",	FALSE,  PARAM_BOOL,	&(s.flags),		F_TEMPORARY },
		{ "trim",	FALSE,  PARAM_BOOL,	&(s.flags),		F_TRIM },
		{ "listenaddr", FALSE,  PARAM_STRING,   &(s.listenaddr),	0 },
		{ "maxconnections", FALSE, PARAM_INT,	&(s.max_connections),	0 },
	};
	const int lp_size=sizeof(lp)/sizeof(PARAM);
        struct generic_conf genconftmp;
	PARAM gp[] = {
		{ "user",	FALSE, PARAM_STRING,	&(genconftmp.user),       0 },
		{ "group",	FALSE, PARAM_STRING,	&(genconftmp.group),      0 },
		{ "oldstyle",	FALSE, PARAM_BOOL,	&(genconftmp.flags),      F_OLDSTYLE },
		{ "listenaddr", FALSE, PARAM_STRING,	&(genconftmp.modernaddr), 0 },
		{ "port", 	FALSE, PARAM_STRING,	&(genconftmp.modernport), 0 },
		{ "includedir", FALSE, PARAM_STRING,	&cfdir,                   0 },
		{ "allowlist",  FALSE, PARAM_BOOL,	&(genconftmp.flags),      F_LIST },
	};
	PARAM* p=gp;
	int p_size=sizeof(gp)/sizeof(PARAM);
	GKeyFile *cfile;
	GError *err = NULL;
	const char *err_msg=NULL;
	GArray *retval=NULL;
	gchar **groups;
	gboolean bval;
	gint ival;
	gint64 i64val;
	gchar* sval;
	gchar* startgroup;
	gint i;
	gint j;

        memset(&genconftmp, 0, sizeof(struct generic_conf));

        if (genconf) {
                /* Use the passed configuration values as defaults. The
                 * parsing algorithm below updates all parameter targets
                 * found from configuration files. */
                memcpy(&genconftmp, genconf, sizeof(struct generic_conf));
        }

	cfile = g_key_file_new();
	retval = g_array_new(FALSE, TRUE, sizeof(SERVER));
	if(!g_key_file_load_from_file(cfile, f, G_KEY_FILE_KEEP_COMMENTS |
			G_KEY_FILE_KEEP_TRANSLATIONS, &err)) {
		g_set_error(e, NBDS_ERR, NBDS_ERR_CFILE_NOTFOUND, "Could not open config file %s: %s",
				f, err->message);
		g_key_file_free(cfile);
		return retval;
	}
	startgroup = g_key_file_get_start_group(cfile);
	if((!startgroup || strcmp(startgroup, "generic")) && genconf) {
		g_set_error(e, NBDS_ERR, NBDS_ERR_CFILE_MISSING_GENERIC, "Config file does not contain the [generic] group!");
		g_key_file_free(cfile);
		return NULL;
	}
	groups = g_key_file_get_groups(cfile, NULL);
	for(i=0;groups[i];i++) {
		memset(&s, '\0', sizeof(SERVER));

		/* After the [generic] group or when we're parsing an include
		 * directory, start parsing exports */
		if(i==1 || !genconf) {
			p=lp;
			p_size=lp_size;
			if(!(glob_flags & F_OLDSTYLE)) {
				lp[1].required = FALSE;
			}
		} 
		for(j=0;j<p_size;j++) {
			assert(p[j].target != NULL);
			assert(p[j].ptype==PARAM_INT||p[j].ptype==PARAM_STRING||p[j].ptype==PARAM_BOOL||p[j].ptype==PARAM_INT64);
			switch(p[j].ptype) {
				case PARAM_INT:
					ival = g_key_file_get_integer(cfile,
								groups[i],
								p[j].paramname,
								&err);
					if(!err) {
						*((gint*)p[j].target) = ival;
					}
					break;
				case PARAM_INT64:
					i64val = g_key_file_get_int64(cfile,
								groups[i],
								p[j].paramname,
								&err);
					if(!err) {
						*((gint64*)p[j].target) = i64val;
					}
					break;
				case PARAM_STRING:
					sval = g_key_file_get_string(cfile,
								groups[i],
								p[j].paramname,
								&err);
					if(!err) {
						*((gchar**)p[j].target) = sval;
					}
					break;
				case PARAM_BOOL:
					bval = g_key_file_get_boolean(cfile,
							groups[i],
							p[j].paramname, &err);
					if(!err) {
						if(bval) {
							*((gint*)p[j].target) |= p[j].flagval;
						} else {
							*((gint*)p[j].target) &= ~(p[j].flagval);
						}
					}
					break;
			}
			if(err) {
				if(err->code == G_KEY_FILE_ERROR_KEY_NOT_FOUND) {
					if(!p[j].required) {
						/* Ignore not-found error for optional values */
						g_clear_error(&err);
						continue;
					} else {
						err_msg = MISSING_REQUIRED_ERROR;
					}
				} else {
					err_msg = DEFAULT_ERROR;
				}
				g_set_error(e, NBDS_ERR, NBDS_ERR_CFILE_VALUE_INVALID, err_msg, p[j].paramname, groups[i], err->message);
				g_array_free(retval, TRUE);
				g_error_free(err);
				g_key_file_free(cfile);
				return NULL;
			}
		}
		if(virtstyle) {
			if(!strncmp(virtstyle, "none", 4)) {
				s.virtstyle=VIRT_NONE;
			} else if(!strncmp(virtstyle, "ipliteral", 9)) {
				s.virtstyle=VIRT_IPLIT;
			} else if(!strncmp(virtstyle, "iphash", 6)) {
				s.virtstyle=VIRT_IPHASH;
			} else if(!strncmp(virtstyle, "cidrhash", 8)) {
				s.virtstyle=VIRT_CIDR;
				if(strlen(virtstyle)<10) {
					g_set_error(e, NBDS_ERR, NBDS_ERR_CFILE_VALUE_INVALID, "Invalid value %s for parameter virtstyle in group %s: missing length", virtstyle, groups[i]);
					g_array_free(retval, TRUE);
					g_key_file_free(cfile);
					return NULL;
				}
				s.cidrlen=strtol(virtstyle+8, NULL, 0);
			} else {
				g_set_error(e, NBDS_ERR, NBDS_ERR_CFILE_VALUE_INVALID, "Invalid value %s for parameter virtstyle in group %s", virtstyle, groups[i]);
				g_array_free(retval, TRUE);
				g_key_file_free(cfile);
				return NULL;
			}
		} else {
			s.virtstyle=VIRT_IPLIT;
		}
		if(s.port && !(glob_flags & F_OLDSTYLE)) {
			g_warning("A port was specified, but oldstyle exports were not requested. This may not do what you expect.");
			g_warning("Please read 'man 5 nbd-server' and search for oldstyle for more info");
		}
		/* Don't need to free this, it's not our string */
		virtstyle=NULL;
		/* Don't append values for the [generic] group */
		if(i>0 || !genconf) {
			s.socket_family = AF_UNSPEC;
			s.servename = groups[i];

			append_serve(&s, retval);
		}
#ifndef WITH_SDP
		if(s.flags & F_SDP) {
			g_set_error(e, NBDS_ERR, NBDS_ERR_CFILE_VALUE_UNSUPPORTED, "This nbd-server was built without support for SDP, yet group %s uses it", groups[i]);
			g_array_free(retval, TRUE);
			g_key_file_free(cfile);
			return NULL;
		}
#endif
	}
	g_key_file_free(cfile);
	if(cfdir) {
		GArray* extra = do_cfile_dir(cfdir, e);
		if(extra) {
			retval = g_array_append_vals(retval, extra->data, extra->len);
			i+=extra->len;
			g_array_free(extra, TRUE);
		} else {
			if(*e) {
				g_array_free(retval, TRUE);
				return NULL;
			}
		}
	}
	if(i==1 && genconf) {
		g_set_error(e, NBDS_ERR, NBDS_ERR_CFILE_NO_EXPORTS, "The config file does not specify any exports");
	}

        if (genconf) {
                /* Return the updated generic configuration through the
                 * pointer parameter. */
                memcpy(genconf, &genconftmp, sizeof(struct generic_conf));
        }

	return retval;
}
