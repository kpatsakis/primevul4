static unsigned int skcipher_poll(struct file *file, struct socket *sock,
				  poll_table *wait)
{
	struct sock *sk = sock->sk;
	struct alg_sock *ask = alg_sk(sk);
	struct skcipher_ctx *ctx = ask->private;
	unsigned int mask;

	sock_poll_wait(file, sk_sleep(sk), wait);
	mask = 0;

	if (ctx->used)
		mask |= POLLIN | POLLRDNORM;

	if (skcipher_writable(sk))
		mask |= POLLOUT | POLLWRNORM | POLLWRBAND;

	return mask;
}
