static int format_peers(char **pdata, unsigned int *len)
{
	struct booth_site *s;
	char *data, *cp;
	char time_str[64];
	int i, alloc;

	*pdata = NULL;
	*len = 0;

	alloc = booth_conf->site_count * (BOOTH_NAME_LEN + 256);
	data = malloc(alloc);
	if (!data)
		return -ENOMEM;

	cp = data;
	foreach_node(i, s) {
		if (s == local)
			continue;
		strftime(time_str, sizeof(time_str), "%F %T",
			localtime(&s->last_recv));
		cp += snprintf(cp,
				alloc - (cp - data),
				"%-12s %s, last recv: %s\n",
				type_to_string(s->type),
				s->addr_string,
				time_str);
		cp += snprintf(cp,
				alloc - (cp - data),
				"\tSent pkts:%u error:%u resends:%u\n",
				s->sent_cnt,
				s->sent_err_cnt,
				s->resend_cnt);
		cp += snprintf(cp,
				alloc - (cp - data),
				"\tRecv pkts:%u error:%u authfail:%u invalid:%u\n\n",
				s->recv_cnt,
				s->recv_err_cnt,
				s->sec_cnt,
				s->invalid_cnt);
		if (alloc - (cp - data) <= 0) {
			free(data);
			return -ENOMEM;
		}
	}

	*pdata = data;
	*len = cp - data;

	return 0;
}