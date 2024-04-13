static void arcmsr_iop2drv_data_wrote_handle(struct AdapterControlBlock *acb)
{
	unsigned long flags;
	struct QBUFFER __iomem  *prbuffer;
	int32_t buf_empty_len;

	spin_lock_irqsave(&acb->rqbuffer_lock, flags);
	prbuffer = arcmsr_get_iop_rqbuffer(acb);
	buf_empty_len = (acb->rqbuf_putIndex - acb->rqbuf_getIndex - 1) &
		(ARCMSR_MAX_QBUFFER - 1);
	if (buf_empty_len >= readl(&prbuffer->data_len)) {
		if (arcmsr_Read_iop_rqbuffer_data(acb, prbuffer) == 0)
			acb->acb_flags |= ACB_F_IOPDATA_OVERFLOW;
	} else
		acb->acb_flags |= ACB_F_IOPDATA_OVERFLOW;
	spin_unlock_irqrestore(&acb->rqbuffer_lock, flags);
}
