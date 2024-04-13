arcmsr_Read_iop_rqbuffer_data(struct AdapterControlBlock *acb,
	struct QBUFFER __iomem *prbuffer) {

	uint8_t *pQbuffer;
	uint8_t __iomem *iop_data;
	uint32_t iop_len;

	if (acb->adapter_type & (ACB_ADAPTER_TYPE_C | ACB_ADAPTER_TYPE_D))
		return arcmsr_Read_iop_rqbuffer_in_DWORD(acb, prbuffer);
	iop_data = (uint8_t __iomem *)prbuffer->data;
	iop_len = readl(&prbuffer->data_len);
	while (iop_len > 0) {
		pQbuffer = &acb->rqbuffer[acb->rqbuf_putIndex];
		*pQbuffer = readb(iop_data);
		acb->rqbuf_putIndex++;
		acb->rqbuf_putIndex %= ARCMSR_MAX_QBUFFER;
		iop_data++;
		iop_len--;
	}
	arcmsr_iop_message_read(acb);
	return 1;
}
