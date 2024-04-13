static irda_queue_t *dequeue_general(irda_queue_t **queue, irda_queue_t* element)
{
	irda_queue_t *ret;

	pr_debug("dequeue_general()\n");

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
		 *  Remove specific element.
		 */
		element->q_prev->q_next = element->q_next;
		element->q_next->q_prev = element->q_prev;
		if ( (*queue) == element)
			(*queue) = element->q_next;
	}

	/*
	 * Return the removed entry (or NULL of queue was empty).
	 */
	return ret;
}
