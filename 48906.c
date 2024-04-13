static int splice_pipe_to_pipe(struct pipe_inode_info *ipipe,
			       struct pipe_inode_info *opipe,
			       size_t len, unsigned int flags)
{
	struct pipe_buffer *ibuf, *obuf;
	int ret = 0, nbuf;
	bool input_wakeup = false;


retry:
	ret = ipipe_prep(ipipe, flags);
	if (ret)
		return ret;

	ret = opipe_prep(opipe, flags);
	if (ret)
		return ret;

	/*
	 * Potential ABBA deadlock, work around it by ordering lock
	 * grabbing by pipe info address. Otherwise two different processes
	 * could deadlock (one doing tee from A -> B, the other from B -> A).
	 */
	pipe_double_lock(ipipe, opipe);

	do {
		if (!opipe->readers) {
			send_sig(SIGPIPE, current, 0);
			if (!ret)
				ret = -EPIPE;
			break;
		}

		if (!ipipe->nrbufs && !ipipe->writers)
			break;

		/*
		 * Cannot make any progress, because either the input
		 * pipe is empty or the output pipe is full.
		 */
		if (!ipipe->nrbufs || opipe->nrbufs >= opipe->buffers) {
			/* Already processed some buffers, break */
			if (ret)
				break;

			if (flags & SPLICE_F_NONBLOCK) {
				ret = -EAGAIN;
				break;
			}

			/*
			 * We raced with another reader/writer and haven't
			 * managed to process any buffers.  A zero return
			 * value means EOF, so retry instead.
			 */
			pipe_unlock(ipipe);
			pipe_unlock(opipe);
			goto retry;
		}

		ibuf = ipipe->bufs + ipipe->curbuf;
		nbuf = (opipe->curbuf + opipe->nrbufs) & (opipe->buffers - 1);
		obuf = opipe->bufs + nbuf;

		if (len >= ibuf->len) {
			/*
			 * Simply move the whole buffer from ipipe to opipe
			 */
			*obuf = *ibuf;
			ibuf->ops = NULL;
			opipe->nrbufs++;
			ipipe->curbuf = (ipipe->curbuf + 1) & (ipipe->buffers - 1);
			ipipe->nrbufs--;
			input_wakeup = true;
		} else {
			/*
			 * Get a reference to this pipe buffer,
			 * so we can copy the contents over.
			 */
			ibuf->ops->get(ipipe, ibuf);
			*obuf = *ibuf;

			/*
			 * Don't inherit the gift flag, we need to
			 * prevent multiple steals of this page.
			 */
			obuf->flags &= ~PIPE_BUF_FLAG_GIFT;

			obuf->len = len;
			opipe->nrbufs++;
			ibuf->offset += obuf->len;
			ibuf->len -= obuf->len;
		}
		ret += obuf->len;
		len -= obuf->len;
	} while (len);

	pipe_unlock(ipipe);
	pipe_unlock(opipe);

	/*
	 * If we put data in the output pipe, wakeup any potential readers.
	 */
	if (ret > 0)
		wakeup_pipe_readers(opipe);

	if (input_wakeup)
		wakeup_pipe_writers(ipipe);

	return ret;
}
