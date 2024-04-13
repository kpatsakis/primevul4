static int arcmsr_alloc_ccb_pool(struct AdapterControlBlock *acb)
{
	struct pci_dev *pdev = acb->pdev;
	void *dma_coherent;
	dma_addr_t dma_coherent_handle;
	struct CommandControlBlock *ccb_tmp;
	int i = 0, j = 0;
	dma_addr_t cdb_phyaddr;
	unsigned long roundup_ccbsize;
	unsigned long max_xfer_len;
	unsigned long max_sg_entrys;
	uint32_t  firm_config_version;

	for (i = 0; i < ARCMSR_MAX_TARGETID; i++)
		for (j = 0; j < ARCMSR_MAX_TARGETLUN; j++)
			acb->devstate[i][j] = ARECA_RAID_GONE;

	max_xfer_len = ARCMSR_MAX_XFER_LEN;
	max_sg_entrys = ARCMSR_DEFAULT_SG_ENTRIES;
	firm_config_version = acb->firm_cfg_version;
	if((firm_config_version & 0xFF) >= 3){
		max_xfer_len = (ARCMSR_CDB_SG_PAGE_LENGTH << ((firm_config_version >> 8) & 0xFF)) * 1024;/* max 4M byte */
		max_sg_entrys = (max_xfer_len/4096);
	}
	acb->host->max_sectors = max_xfer_len/512;
	acb->host->sg_tablesize = max_sg_entrys;
	roundup_ccbsize = roundup(sizeof(struct CommandControlBlock) + (max_sg_entrys - 1) * sizeof(struct SG64ENTRY), 32);
	acb->uncache_size = roundup_ccbsize * ARCMSR_MAX_FREECCB_NUM;
	dma_coherent = dma_alloc_coherent(&pdev->dev, acb->uncache_size, &dma_coherent_handle, GFP_KERNEL);
	if(!dma_coherent){
		printk(KERN_NOTICE "arcmsr%d: dma_alloc_coherent got error\n", acb->host->host_no);
		return -ENOMEM;
	}
	acb->dma_coherent = dma_coherent;
	acb->dma_coherent_handle = dma_coherent_handle;
	memset(dma_coherent, 0, acb->uncache_size);
	ccb_tmp = dma_coherent;
	acb->vir2phy_offset = (unsigned long)dma_coherent - (unsigned long)dma_coherent_handle;
	for(i = 0; i < ARCMSR_MAX_FREECCB_NUM; i++){
		cdb_phyaddr = dma_coherent_handle + offsetof(struct CommandControlBlock, arcmsr_cdb);
		switch (acb->adapter_type) {
		case ACB_ADAPTER_TYPE_A:
		case ACB_ADAPTER_TYPE_B:
			ccb_tmp->cdb_phyaddr = cdb_phyaddr >> 5;
			break;
		case ACB_ADAPTER_TYPE_C:
		case ACB_ADAPTER_TYPE_D:
			ccb_tmp->cdb_phyaddr = cdb_phyaddr;
			break;
		}
		acb->pccb_pool[i] = ccb_tmp;
		ccb_tmp->acb = acb;
		INIT_LIST_HEAD(&ccb_tmp->list);
		list_add_tail(&ccb_tmp->list, &acb->ccb_free_list);
		ccb_tmp = (struct CommandControlBlock *)((unsigned long)ccb_tmp + roundup_ccbsize);
		dma_coherent_handle = dma_coherent_handle + roundup_ccbsize;
	}
	return 0;
}
