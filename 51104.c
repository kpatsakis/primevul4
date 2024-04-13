static bool arcmsr_remap_pciregion(struct AdapterControlBlock *acb)
{
	struct pci_dev *pdev = acb->pdev;
	switch (acb->adapter_type){
	case ACB_ADAPTER_TYPE_A:{
		acb->pmuA = ioremap(pci_resource_start(pdev,0), pci_resource_len(pdev,0));
		if (!acb->pmuA) {
			printk(KERN_NOTICE "arcmsr%d: memory mapping region fail \n", acb->host->host_no);
			return false;
		}
		break;
	}
	case ACB_ADAPTER_TYPE_B:{
		void __iomem *mem_base0, *mem_base1;
		mem_base0 = ioremap(pci_resource_start(pdev, 0), pci_resource_len(pdev, 0));
		if (!mem_base0) {
			printk(KERN_NOTICE "arcmsr%d: memory mapping region fail \n", acb->host->host_no);
			return false;
		}
		mem_base1 = ioremap(pci_resource_start(pdev, 2), pci_resource_len(pdev, 2));
		if (!mem_base1) {
			iounmap(mem_base0);
			printk(KERN_NOTICE "arcmsr%d: memory mapping region fail \n", acb->host->host_no);
			return false;
		}
		acb->mem_base0 = mem_base0;
		acb->mem_base1 = mem_base1;
		break;
	}
	case ACB_ADAPTER_TYPE_C:{
		acb->pmuC = ioremap_nocache(pci_resource_start(pdev, 1), pci_resource_len(pdev, 1));
		if (!acb->pmuC) {
			printk(KERN_NOTICE "arcmsr%d: memory mapping region fail \n", acb->host->host_no);
			return false;
		}
		if (readl(&acb->pmuC->outbound_doorbell) & ARCMSR_HBCMU_IOP2DRV_MESSAGE_CMD_DONE) {
			writel(ARCMSR_HBCMU_IOP2DRV_MESSAGE_CMD_DONE_DOORBELL_CLEAR, &acb->pmuC->outbound_doorbell_clear);/*clear interrupt*/
			return true;
		}
		break;
	}
	case ACB_ADAPTER_TYPE_D: {
		void __iomem *mem_base0;
		unsigned long addr, range, flags;

		addr = (unsigned long)pci_resource_start(pdev, 0);
		range = pci_resource_len(pdev, 0);
		flags = pci_resource_flags(pdev, 0);
		mem_base0 = ioremap(addr, range);
		if (!mem_base0) {
			pr_notice("arcmsr%d: memory mapping region fail\n",
				acb->host->host_no);
			return false;
		}
		acb->mem_base0 = mem_base0;
		break;
		}
	}
	return true;
}
