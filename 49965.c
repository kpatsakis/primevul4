static void bsg_add_command(struct bsg_device *bd, struct request_queue *q,
			    struct bsg_command *bc, struct request *rq)
{
	int at_head = (0 == (bc->hdr.flags & BSG_FLAG_Q_AT_TAIL));

	/*
	 * add bc command to busy queue and submit rq for io
	 */
	bc->rq = rq;
	bc->bio = rq->bio;
	if (rq->next_rq)
		bc->bidi_bio = rq->next_rq->bio;
	bc->hdr.duration = jiffies;
	spin_lock_irq(&bd->lock);
	list_add_tail(&bc->list, &bd->busy_list);
	spin_unlock_irq(&bd->lock);

	dprintk("%s: queueing rq %p, bc %p\n", bd->name, rq, bc);

	rq->end_io_data = bc;
	blk_execute_rq_nowait(q, NULL, rq, at_head, bsg_rq_end_io);
}
