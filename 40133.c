static int skcipher_sendmsg(struct kiocb *unused, struct socket *sock,
			    struct msghdr *msg, size_t size)
{
	struct sock *sk = sock->sk;
	struct alg_sock *ask = alg_sk(sk);
	struct skcipher_ctx *ctx = ask->private;
	struct crypto_ablkcipher *tfm = crypto_ablkcipher_reqtfm(&ctx->req);
	unsigned ivsize = crypto_ablkcipher_ivsize(tfm);
	struct skcipher_sg_list *sgl;
	struct af_alg_control con = {};
	long copied = 0;
	bool enc = 0;
	int err;
	int i;

	if (msg->msg_controllen) {
		err = af_alg_cmsg_send(msg, &con);
		if (err)
			return err;

		switch (con.op) {
		case ALG_OP_ENCRYPT:
			enc = 1;
			break;
		case ALG_OP_DECRYPT:
			enc = 0;
			break;
		default:
			return -EINVAL;
		}

		if (con.iv && con.iv->ivlen != ivsize)
			return -EINVAL;
	}

	err = -EINVAL;

	lock_sock(sk);
	if (!ctx->more && ctx->used)
		goto unlock;

	if (!ctx->used) {
		ctx->enc = enc;
		if (con.iv)
			memcpy(ctx->iv, con.iv->iv, ivsize);
	}

	while (size) {
		struct scatterlist *sg;
		unsigned long len = size;
		int plen;

		if (ctx->merge) {
			sgl = list_entry(ctx->tsgl.prev,
					 struct skcipher_sg_list, list);
			sg = sgl->sg + sgl->cur - 1;
			len = min_t(unsigned long, len,
				    PAGE_SIZE - sg->offset - sg->length);

			err = memcpy_fromiovec(page_address(sg_page(sg)) +
					       sg->offset + sg->length,
					       msg->msg_iov, len);
			if (err)
				goto unlock;

			sg->length += len;
			ctx->merge = (sg->offset + sg->length) &
				     (PAGE_SIZE - 1);

			ctx->used += len;
			copied += len;
			size -= len;
			continue;
		}

		if (!skcipher_writable(sk)) {
			err = skcipher_wait_for_wmem(sk, msg->msg_flags);
			if (err)
				goto unlock;
		}

		len = min_t(unsigned long, len, skcipher_sndbuf(sk));

		err = skcipher_alloc_sgl(sk);
		if (err)
			goto unlock;

		sgl = list_entry(ctx->tsgl.prev, struct skcipher_sg_list, list);
		sg = sgl->sg;
		do {
			i = sgl->cur;
			plen = min_t(int, len, PAGE_SIZE);

			sg_assign_page(sg + i, alloc_page(GFP_KERNEL));
			err = -ENOMEM;
			if (!sg_page(sg + i))
				goto unlock;

			err = memcpy_fromiovec(page_address(sg_page(sg + i)),
					       msg->msg_iov, plen);
			if (err) {
				__free_page(sg_page(sg + i));
				sg_assign_page(sg + i, NULL);
				goto unlock;
			}

			sg[i].length = plen;
			len -= plen;
			ctx->used += plen;
			copied += plen;
			size -= plen;
			sgl->cur++;
		} while (len && sgl->cur < MAX_SGL_ENTS);

		ctx->merge = plen & (PAGE_SIZE - 1);
	}

	err = 0;

	ctx->more = msg->msg_flags & MSG_MORE;
	if (!ctx->more && !list_empty(&ctx->tsgl))
		sgl = list_entry(ctx->tsgl.prev, struct skcipher_sg_list, list);

unlock:
	skcipher_data_wakeup(sk);
	release_sock(sk);

	return copied ?: err;
}
