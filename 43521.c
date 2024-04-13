char *recv_line(struct pool *pool)
{
	char *tok, *sret = NULL;
	ssize_t len, buflen;
	int waited = 0;

	if (!strstr(pool->sockbuf, "\n")) {
		struct timeval rstart, now;

		cgtime(&rstart);
		if (!socket_full(pool, DEFAULT_SOCKWAIT)) {
			applog(LOG_DEBUG, "Timed out waiting for data on socket_full");
			goto out;
		}

		do {
			char s[RBUFSIZE];
			size_t slen;
			ssize_t n;

			memset(s, 0, RBUFSIZE);
			n = recv(pool->sock, s, RECVSIZE, 0);
			if (!n) {
				applog(LOG_DEBUG, "Socket closed waiting in recv_line");
				suspend_stratum(pool);
				break;
			}
			cgtime(&now);
			waited = tdiff(&now, &rstart);
			if (n < 0) {
				if (!sock_blocks() || !socket_full(pool, DEFAULT_SOCKWAIT - waited)) {
					applog(LOG_DEBUG, "Failed to recv sock in recv_line");
					suspend_stratum(pool);
					break;
				}
			} else {
				slen = strlen(s);
				recalloc_sock(pool, slen);
				strcat(pool->sockbuf, s);
			}
		} while (waited < DEFAULT_SOCKWAIT && !strstr(pool->sockbuf, "\n"));
	}

	buflen = strlen(pool->sockbuf);
	tok = strtok(pool->sockbuf, "\n");
	if (!tok) {
		applog(LOG_DEBUG, "Failed to parse a \\n terminated string in recv_line");
		goto out;
	}
	sret = strdup(tok);
	len = strlen(sret);

	/* Copy what's left in the buffer after the \n, including the
	 * terminating \0 */
	if (buflen > len + 1)
		memmove(pool->sockbuf, pool->sockbuf + len + 1, buflen - len + 1);
	else
		strcpy(pool->sockbuf, "");

	pool->sgminer_pool_stats.times_received++;
	pool->sgminer_pool_stats.bytes_received += len;
	pool->sgminer_pool_stats.net_bytes_received += len;
out:
	if (!sret)
		clear_sock(pool);
	else if (opt_protocol)
		applog(LOG_DEBUG, "RECVD: %s", sret);
	return sret;
}
