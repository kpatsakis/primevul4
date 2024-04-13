int crypto_enqueue_request(struct crypto_queue *queue,
			   struct crypto_async_request *request)
{
	int err = -EINPROGRESS;

	if (unlikely(queue->qlen >= queue->max_qlen)) {
		err = -EBUSY;
		if (!(request->flags & CRYPTO_TFM_REQ_MAY_BACKLOG))
			goto out;
		if (queue->backlog == &queue->list)
			queue->backlog = &request->list;
	}

	queue->qlen++;
	list_add_tail(&request->list, &queue->list);

out:
	return err;
}
