void arcmsr_clear_iop2drv_rqueue_buffer(struct AdapterControlBlock *acb)
{
	uint32_t	i;

	if (acb->acb_flags & ACB_F_IOPDATA_OVERFLOW) {
		for (i = 0; i < 15; i++) {
			if (acb->acb_flags & ACB_F_IOPDATA_OVERFLOW) {
				acb->acb_flags &= ~ACB_F_IOPDATA_OVERFLOW;
				acb->rqbuf_getIndex = 0;
				acb->rqbuf_putIndex = 0;
				arcmsr_iop_message_read(acb);
				mdelay(30);
			} else if (acb->rqbuf_getIndex !=
				   acb->rqbuf_putIndex) {
				acb->rqbuf_getIndex = 0;
				acb->rqbuf_putIndex = 0;
				mdelay(30);
			} else
				break;
		}
	}
}
