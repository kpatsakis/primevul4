static bool arcmsr_hbaD_get_config(struct AdapterControlBlock *acb)
{
	char *acb_firm_model = acb->firm_model;
	char *acb_firm_version = acb->firm_version;
	char *acb_device_map = acb->device_map;
	char __iomem *iop_firm_model;
	char __iomem *iop_firm_version;
	char __iomem *iop_device_map;
	u32 count;
	struct MessageUnit_D *reg = acb->pmuD;

	iop_firm_model = (char __iomem *)(&reg->msgcode_rwbuffer[15]);
	iop_firm_version = (char __iomem *)(&reg->msgcode_rwbuffer[17]);
	iop_device_map = (char __iomem *)(&reg->msgcode_rwbuffer[21]);
	if (readl(acb->pmuD->outbound_doorbell) &
		ARCMSR_ARC1214_IOP2DRV_MESSAGE_CMD_DONE) {
		writel(ARCMSR_ARC1214_IOP2DRV_MESSAGE_CMD_DONE,
			acb->pmuD->outbound_doorbell);/*clear interrupt*/
	}
	/* post "get config" instruction */
	writel(ARCMSR_INBOUND_MESG0_GET_CONFIG, reg->inbound_msgaddr0);
	/* wait message ready */
	if (!arcmsr_hbaD_wait_msgint_ready(acb)) {
		pr_notice("arcmsr%d: wait get adapter firmware "
			"miscellaneous data timeout\n", acb->host->host_no);
		return false;
	}
	count = 8;
	while (count) {
		*acb_firm_model = readb(iop_firm_model);
		acb_firm_model++;
		iop_firm_model++;
		count--;
	}
	count = 16;
	while (count) {
		*acb_firm_version = readb(iop_firm_version);
		acb_firm_version++;
		iop_firm_version++;
		count--;
	}
	count = 16;
	while (count) {
		*acb_device_map = readb(iop_device_map);
		acb_device_map++;
		iop_device_map++;
		count--;
	}
	acb->signature = readl(&reg->msgcode_rwbuffer[0]);
	/*firm_signature,1,00-03*/
	acb->firm_request_len = readl(&reg->msgcode_rwbuffer[1]);
	/*firm_request_len,1,04-07*/
	acb->firm_numbers_queue = readl(&reg->msgcode_rwbuffer[2]);
	/*firm_numbers_queue,2,08-11*/
	acb->firm_sdram_size = readl(&reg->msgcode_rwbuffer[3]);
	/*firm_sdram_size,3,12-15*/
	acb->firm_hd_channels = readl(&reg->msgcode_rwbuffer[4]);
	/*firm_hd_channels,4,16-19*/
	acb->firm_cfg_version = readl(&reg->msgcode_rwbuffer[25]);
	pr_notice("Areca RAID Controller%d: Model %s, F/W %s\n",
		acb->host->host_no,
		acb->firm_model,
		acb->firm_version);
	return true;
}
