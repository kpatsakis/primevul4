static void bsg_free_command(struct bsg_command *bc)
{
	struct bsg_device *bd = bc->bd;
	unsigned long flags;

	kmem_cache_free(bsg_cmd_cachep, bc);

	spin_lock_irqsave(&bd->lock, flags);
	bd->queued_cmds--;
	spin_unlock_irqrestore(&bd->lock, flags);

	wake_up(&bd->wq_free);
}
