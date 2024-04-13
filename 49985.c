static void bsg_rq_end_io(struct request *rq, int uptodate)
{
	struct bsg_command *bc = rq->end_io_data;
	struct bsg_device *bd = bc->bd;
	unsigned long flags;

	dprintk("%s: finished rq %p bc %p, bio %p stat %d\n",
		bd->name, rq, bc, bc->bio, uptodate);

	bc->hdr.duration = jiffies_to_msecs(jiffies - bc->hdr.duration);

	spin_lock_irqsave(&bd->lock, flags);
	list_move_tail(&bc->list, &bd->done_list);
	bd->done_cmds++;
	spin_unlock_irqrestore(&bd->lock, flags);

	wake_up(&bd->wq_done);
}
