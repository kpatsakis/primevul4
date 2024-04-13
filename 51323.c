int sock_wake_async(struct socket_wq *wq, int how, int band)
{
	if (!wq || !wq->fasync_list)
		return -1;

	switch (how) {
	case SOCK_WAKE_WAITD:
		if (test_bit(SOCKWQ_ASYNC_WAITDATA, &wq->flags))
			break;
		goto call_kill;
	case SOCK_WAKE_SPACE:
		if (!test_and_clear_bit(SOCKWQ_ASYNC_NOSPACE, &wq->flags))
			break;
		/* fall through */
	case SOCK_WAKE_IO:
call_kill:
		kill_fasync(&wq->fasync_list, SIGIO, band);
		break;
	case SOCK_WAKE_URG:
		kill_fasync(&wq->fasync_list, SIGURG, band);
	}

	return 0;
}
