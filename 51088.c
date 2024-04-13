static void arcmsr_iop2drv_data_read_handle(struct AdapterControlBlock *acb)
{
	unsigned long flags;

	spin_lock_irqsave(&acb->wqbuffer_lock, flags);
	acb->acb_flags |= ACB_F_MESSAGE_WQBUFFER_READED;
	if (acb->wqbuf_getIndex != acb->wqbuf_putIndex)
		arcmsr_write_ioctldata2iop(acb);
	if (acb->wqbuf_getIndex == acb->wqbuf_putIndex)
		acb->acb_flags |= ACB_F_MESSAGE_WQBUFFER_CLEARED;
	spin_unlock_irqrestore(&acb->wqbuffer_lock, flags);
}
