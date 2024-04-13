static void arcmsr_write_ioctldata2iop_in_DWORD(struct AdapterControlBlock *acb)
{
	uint8_t *pQbuffer;
	struct QBUFFER __iomem *pwbuffer;
	uint8_t *buf1 = NULL;
	uint32_t __iomem *iop_data;
	uint32_t allxfer_len = 0, data_len, *buf2 = NULL, data;

	if (acb->acb_flags & ACB_F_MESSAGE_WQBUFFER_READED) {
		buf1 = kmalloc(128, GFP_ATOMIC);
		buf2 = (uint32_t *)buf1;
		if (buf1 == NULL)
			return;

		acb->acb_flags &= (~ACB_F_MESSAGE_WQBUFFER_READED);
		pwbuffer = arcmsr_get_iop_wqbuffer(acb);
		iop_data = (uint32_t __iomem *)pwbuffer->data;
		while ((acb->wqbuf_getIndex != acb->wqbuf_putIndex)
			&& (allxfer_len < 124)) {
			pQbuffer = &acb->wqbuffer[acb->wqbuf_getIndex];
			*buf1 = *pQbuffer;
			acb->wqbuf_getIndex++;
			acb->wqbuf_getIndex %= ARCMSR_MAX_QBUFFER;
			buf1++;
			allxfer_len++;
		}
		data_len = allxfer_len;
		buf1 = (uint8_t *)buf2;
		while (data_len >= 4) {
			data = *buf2++;
			writel(data, iop_data);
			iop_data++;
			data_len -= 4;
		}
		if (data_len) {
			data = *buf2;
			writel(data, iop_data);
		}
		writel(allxfer_len, &pwbuffer->data_len);
		kfree(buf1);
		arcmsr_iop_message_wrote(acb);
	}
}
