int crypto_tfm_in_queue(struct crypto_queue *queue, struct crypto_tfm *tfm)
{
	struct crypto_async_request *req;

	list_for_each_entry(req, &queue->list, list) {
		if (req->tfm == tfm)
			return 1;
	}

	return 0;
}
