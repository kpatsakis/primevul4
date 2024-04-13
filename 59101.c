static void enqueue_first(irda_queue_t **queue, irda_queue_t* element)
{

	/*
	 * Check if queue is empty.
	 */
	if ( *queue == NULL ) {
		/*
		 * Queue is empty.  Insert one element into the queue.
		 */
		element->q_next = element->q_prev = *queue = element;

	} else {
		/*
		 * Queue is not empty.  Insert element into front of queue.
		 */
		element->q_next          = (*queue);
		(*queue)->q_prev->q_next = element;
		element->q_prev          = (*queue)->q_prev;
		(*queue)->q_prev         = element;
		(*queue)                 = element;
	}
}
