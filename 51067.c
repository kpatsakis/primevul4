static bool arcmsr_hbaC_get_config(struct AdapterControlBlock *pACB)
{
	uint32_t intmask_org, Index, firmware_state = 0;
	struct MessageUnit_C __iomem *reg = pACB->pmuC;
	char *acb_firm_model = pACB->firm_model;
	char *acb_firm_version = pACB->firm_version;
	char __iomem *iop_firm_model = (char __iomem *)(&reg->msgcode_rwbuffer[15]);    /*firm_model,15,60-67*/
	char __iomem *iop_firm_version = (char __iomem *)(&reg->msgcode_rwbuffer[17]);  /*firm_version,17,68-83*/
	int count;
	/* disable all outbound interrupt */
	intmask_org = readl(&reg->host_int_mask); /* disable outbound message0 int */
	writel(intmask_org|ARCMSR_HBCMU_ALL_INTMASKENABLE, &reg->host_int_mask);
	/* wait firmware ready */
	do {
		firmware_state = readl(&reg->outbound_msgaddr1);
	} while ((firmware_state & ARCMSR_HBCMU_MESSAGE_FIRMWARE_OK) == 0);
	/* post "get config" instruction */
	writel(ARCMSR_INBOUND_MESG0_GET_CONFIG, &reg->inbound_msgaddr0);
	writel(ARCMSR_HBCMU_DRV2IOP_MESSAGE_CMD_DONE, &reg->inbound_doorbell);
	/* wait message ready */
	for (Index = 0; Index < 2000; Index++) {
		if (readl(&reg->outbound_doorbell) & ARCMSR_HBCMU_IOP2DRV_MESSAGE_CMD_DONE) {
			writel(ARCMSR_HBCMU_IOP2DRV_MESSAGE_CMD_DONE_DOORBELL_CLEAR, &reg->outbound_doorbell_clear);/*clear interrupt*/
			break;
		}
		udelay(10);
	} /*max 1 seconds*/
	if (Index >= 2000) {
		printk(KERN_NOTICE "arcmsr%d: wait 'get adapter firmware \
			miscellaneous data' timeout \n", pACB->host->host_no);
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
	pr_notice("Areca RAID Controller%d: Model %s, F/W %s\n",
		pACB->host->host_no,
		pACB->firm_model,
		pACB->firm_version);
	pACB->firm_request_len = readl(&reg->msgcode_rwbuffer[1]);   /*firm_request_len,1,04-07*/
	pACB->firm_numbers_queue = readl(&reg->msgcode_rwbuffer[2]); /*firm_numbers_queue,2,08-11*/
	pACB->firm_sdram_size = readl(&reg->msgcode_rwbuffer[3]);    /*firm_sdram_size,3,12-15*/
	pACB->firm_hd_channels = readl(&reg->msgcode_rwbuffer[4]);  /*firm_ide_channels,4,16-19*/
	pACB->firm_cfg_version = readl(&reg->msgcode_rwbuffer[25]);  /*firm_cfg_version,25,100-103*/
	/*all interrupt service will be enable at arcmsr_iop_init*/
	return true;
}
