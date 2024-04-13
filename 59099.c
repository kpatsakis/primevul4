static irda_queue_t *dequeue_first(irda_queue_t **queue)
{
	irda_queue_t *ret;

	pr_debug("dequeue_first()\n");

	/*
	 * Set return value
	 */
	ret =  *queue;

	if ( *queue == NULL ) {
		/*
		 * Queue was empty.
		 */
	} else if ( (*queue)->q_next == *queue ) {
		/*
		 *  Queue only contained a single element. It will now be
		 *  empty.
		 */
		*queue = NULL;
	} else {
		/*
		 * Queue contained several element.  Remove the first one.
		 */
		(*queue)->q_prev->q_next = (*queue)->q_next;
		(*queue)->q_next->q_prev = (*queue)->q_prev;
		*queue = (*queue)->q_next;
	}

	/*
	 * Return the removed entry (or NULL of queue was empty).
	 */
	return ret;
}
