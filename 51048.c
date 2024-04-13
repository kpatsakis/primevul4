static bool arcmsr_hbaA_get_config(struct AdapterControlBlock *acb)
{
	struct MessageUnit_A __iomem *reg = acb->pmuA;
	char *acb_firm_model = acb->firm_model;
	char *acb_firm_version = acb->firm_version;
	char *acb_device_map = acb->device_map;
	char __iomem *iop_firm_model = (char __iomem *)(&reg->message_rwbuffer[15]);
	char __iomem *iop_firm_version = (char __iomem *)(&reg->message_rwbuffer[17]);
	char __iomem *iop_device_map = (char __iomem *)(&reg->message_rwbuffer[21]);
	int count;
	writel(ARCMSR_INBOUND_MESG0_GET_CONFIG, &reg->inbound_msgaddr0);
	if (!arcmsr_hbaA_wait_msgint_ready(acb)) {
		printk(KERN_NOTICE "arcmsr%d: wait 'get adapter firmware \
			miscellaneous data' timeout \n", acb->host->host_no);
		return false;
	}
	count = 8;
	while (count){
		*acb_firm_model = readb(iop_firm_model);
		acb_firm_model++;
		iop_firm_model++;
		count--;
	}

	count = 16;
	while (count){
		*acb_firm_version = readb(iop_firm_version);
		acb_firm_version++;
		iop_firm_version++;
		count--;
	}

	count=16;
	while(count){
		*acb_device_map = readb(iop_device_map);
		acb_device_map++;
		iop_device_map++;
		count--;
	}
	pr_notice("Areca RAID Controller%d: Model %s, F/W %s\n",
		acb->host->host_no,
		acb->firm_model,
		acb->firm_version);
	acb->signature = readl(&reg->message_rwbuffer[0]);
	acb->firm_request_len = readl(&reg->message_rwbuffer[1]);
	acb->firm_numbers_queue = readl(&reg->message_rwbuffer[2]);
	acb->firm_sdram_size = readl(&reg->message_rwbuffer[3]);
	acb->firm_hd_channels = readl(&reg->message_rwbuffer[4]);
	acb->firm_cfg_version = readl(&reg->message_rwbuffer[25]);  /*firm_cfg_version,25,100-103*/
	return true;
}
