void inet_csk_reqsk_queue_drop(struct sock *sk, struct request_sock *req)
{
	if (reqsk_queue_unlink(&inet_csk(sk)->icsk_accept_queue, req)) {
		reqsk_queue_removed(&inet_csk(sk)->icsk_accept_queue, req);
		reqsk_put(req);
	}
}
