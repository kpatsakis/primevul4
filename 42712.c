static int sctp_wait_for_packet(struct sock * sk, int *err, long *timeo_p)
{
	int error;
	DEFINE_WAIT(wait);

	prepare_to_wait_exclusive(sk->sk_sleep, &wait, TASK_INTERRUPTIBLE);

	/* Socket errors? */
	error = sock_error(sk);
	if (error)
		goto out;

	if (!skb_queue_empty(&sk->sk_receive_queue))
		goto ready;

	/* Socket shut down?  */
	if (sk->sk_shutdown & RCV_SHUTDOWN)
		goto out;

	/* Sequenced packets can come disconnected.  If so we report the
	 * problem.
	 */
	error = -ENOTCONN;

	/* Is there a good reason to think that we may receive some data?  */
	if (list_empty(&sctp_sk(sk)->ep->asocs) && !sctp_sstate(sk, LISTENING))
		goto out;

	/* Handle signals.  */
	if (signal_pending(current))
		goto interrupted;

	/* Let another process have a go.  Since we are going to sleep
	 * anyway.  Note: This may cause odd behaviors if the message
	 * does not fit in the user's buffer, but this seems to be the
	 * only way to honor MSG_DONTWAIT realistically.
	 */
	sctp_release_sock(sk);
	*timeo_p = schedule_timeout(*timeo_p);
	sctp_lock_sock(sk);

ready:
	finish_wait(sk->sk_sleep, &wait);
	return 0;

interrupted:
	error = sock_intr_errno(*timeo_p);

out:
	finish_wait(sk->sk_sleep, &wait);
	*err = error;
	return error;
}
