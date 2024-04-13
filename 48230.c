struct crypto_async_request *crypto_dequeue_request(struct crypto_queue *queue)
{
	return __crypto_dequeue_request(queue, 0);
}
