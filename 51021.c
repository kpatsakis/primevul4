arcmsr_Read_iop_rqbuffer_in_DWORD(struct AdapterControlBlock *acb,
		struct QBUFFER __iomem *prbuffer)
{
	uint8_t *pQbuffer;
	uint8_t *buf1 = NULL;
	uint32_t __iomem *iop_data;
	uint32_t iop_len, data_len, *buf2 = NULL;

	iop_data = (uint32_t __iomem *)prbuffer->data;
	iop_len = readl(&prbuffer->data_len);
	if (iop_len > 0) {
		buf1 = kmalloc(128, GFP_ATOMIC);
		buf2 = (uint32_t *)buf1;
		if (buf1 == NULL)
			return 0;
		data_len = iop_len;
		while (data_len >= 4) {
			*buf2++ = readl(iop_data);
			iop_data++;
			data_len -= 4;
		}
		if (data_len)
			*buf2 = readl(iop_data);
		buf2 = (uint32_t *)buf1;
	}
	while (iop_len > 0) {
		pQbuffer = &acb->rqbuffer[acb->rqbuf_putIndex];
		*pQbuffer = *buf1;
		acb->rqbuf_putIndex++;
		/* if last, index number set it to 0 */
		acb->rqbuf_putIndex %= ARCMSR_MAX_QBUFFER;
		buf1++;
		iop_len--;
	}
	kfree(buf2);
	/* let IOP know data has been read */
	arcmsr_iop_message_read(acb);
	return 1;
}
