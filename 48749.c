struct timeval* dtls1_get_timeout(SSL *s, struct timeval* timeleft)
	{
	struct timeval timenow;

	/* If no timeout is set, just return NULL */
	if (s->d1->next_timeout.tv_sec == 0 && s->d1->next_timeout.tv_usec == 0)
		{
		return NULL;
		}

	/* Get current time */
	get_current_time(&timenow);

	/* If timer already expired, set remaining time to 0 */
	if (s->d1->next_timeout.tv_sec < timenow.tv_sec ||
		(s->d1->next_timeout.tv_sec == timenow.tv_sec &&
		 s->d1->next_timeout.tv_usec <= timenow.tv_usec))
		{
		memset(timeleft, 0, sizeof(struct timeval));
		return timeleft;
		}

	/* Calculate time left until timer expires */
	memcpy(timeleft, &(s->d1->next_timeout), sizeof(struct timeval));
	timeleft->tv_sec -= timenow.tv_sec;
	timeleft->tv_usec -= timenow.tv_usec;
	if (timeleft->tv_usec < 0)
		{
		timeleft->tv_sec--;
		timeleft->tv_usec += 1000000;
		}

	/* If remaining time is less than 15 ms, set it to 0
	 * to prevent issues because of small devergences with
	 * socket timeouts.
	 */
	if (timeleft->tv_sec == 0 && timeleft->tv_usec < 15000)
		{
		memset(timeleft, 0, sizeof(struct timeval));
		}
	

	return timeleft;
	}
