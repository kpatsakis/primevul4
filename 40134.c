static ssize_t skcipher_sendpage(struct socket *sock, struct page *page,
				 int offset, size_t size, int flags)
{
	struct sock *sk = sock->sk;
	struct alg_sock *ask = alg_sk(sk);
	struct skcipher_ctx *ctx = ask->private;
	struct skcipher_sg_list *sgl;
	int err = -EINVAL;

	lock_sock(sk);
	if (!ctx->more && ctx->used)
		goto unlock;

	if (!size)
		goto done;

	if (!skcipher_writable(sk)) {
		err = skcipher_wait_for_wmem(sk, flags);
		if (err)
			goto unlock;
	}

	err = skcipher_alloc_sgl(sk);
	if (err)
		goto unlock;

	ctx->merge = 0;
	sgl = list_entry(ctx->tsgl.prev, struct skcipher_sg_list, list);

	get_page(page);
	sg_set_page(sgl->sg + sgl->cur, page, size, offset);
	sgl->cur++;
	ctx->used += size;

done:
	ctx->more = flags & MSG_MORE;
	if (!ctx->more && !list_empty(&ctx->tsgl))
		sgl = list_entry(ctx->tsgl.prev, struct skcipher_sg_list, list);

unlock:
	skcipher_data_wakeup(sk);
	release_sock(sk);

	return err ?: size;
}
