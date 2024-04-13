arcmsr_write_ioctldata2iop(struct AdapterControlBlock *acb)
{
	uint8_t *pQbuffer;
	struct QBUFFER __iomem *pwbuffer;
	uint8_t __iomem *iop_data;
	int32_t allxfer_len = 0;

	if (acb->adapter_type & (ACB_ADAPTER_TYPE_C | ACB_ADAPTER_TYPE_D)) {
		arcmsr_write_ioctldata2iop_in_DWORD(acb);
		return;
	}
	if (acb->acb_flags & ACB_F_MESSAGE_WQBUFFER_READED) {
		acb->acb_flags &= (~ACB_F_MESSAGE_WQBUFFER_READED);
		pwbuffer = arcmsr_get_iop_wqbuffer(acb);
		iop_data = (uint8_t __iomem *)pwbuffer->data;
		while ((acb->wqbuf_getIndex != acb->wqbuf_putIndex)
			&& (allxfer_len < 124)) {
			pQbuffer = &acb->wqbuffer[acb->wqbuf_getIndex];
			writeb(*pQbuffer, iop_data);
			acb->wqbuf_getIndex++;
			acb->wqbuf_getIndex %= ARCMSR_MAX_QBUFFER;
			iop_data++;
			allxfer_len++;
		}
		writel(allxfer_len, &pwbuffer->data_len);
		arcmsr_iop_message_wrote(acb);
	}
}
