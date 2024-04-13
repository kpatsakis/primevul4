static int arcmsr_bus_reset(struct scsi_cmnd *cmd)
{
	struct AdapterControlBlock *acb;
	uint32_t intmask_org, outbound_doorbell;
	int retry_count = 0;
	int rtn = FAILED;
	acb = (struct AdapterControlBlock *) cmd->device->host->hostdata;
	printk(KERN_ERR "arcmsr: executing bus reset eh.....num_resets = %d, num_aborts = %d \n", acb->num_resets, acb->num_aborts);
	acb->num_resets++;

	switch(acb->adapter_type){
		case ACB_ADAPTER_TYPE_A:{
			if (acb->acb_flags & ACB_F_BUS_RESET){
				long timeout;
				printk(KERN_ERR "arcmsr: there is an  bus reset eh proceeding.......\n");
				timeout = wait_event_timeout(wait_q, (acb->acb_flags & ACB_F_BUS_RESET) == 0, 220*HZ);
				if (timeout) {
					return SUCCESS;
				}
			}
			acb->acb_flags |= ACB_F_BUS_RESET;
			if (!arcmsr_iop_reset(acb)) {
				struct MessageUnit_A __iomem *reg;
				reg = acb->pmuA;
				arcmsr_hardware_reset(acb);
				acb->acb_flags &= ~ACB_F_IOP_INITED;
sleep_again:
				ssleep(ARCMSR_SLEEPTIME);
				if ((readl(&reg->outbound_msgaddr1) & ARCMSR_OUTBOUND_MESG1_FIRMWARE_OK) == 0) {
					printk(KERN_ERR "arcmsr%d: waiting for hw bus reset return, retry=%d\n", acb->host->host_no, retry_count);
					if (retry_count > ARCMSR_RETRYCOUNT) {
						acb->fw_flag = FW_DEADLOCK;
						printk(KERN_ERR "arcmsr%d: waiting for hw bus reset return, RETRY TERMINATED!!\n", acb->host->host_no);
						return FAILED;
					}
					retry_count++;
					goto sleep_again;
				}
				acb->acb_flags |= ACB_F_IOP_INITED;
				/* disable all outbound interrupt */
				intmask_org = arcmsr_disable_outbound_ints(acb);
				arcmsr_get_firmware_spec(acb);
				arcmsr_start_adapter_bgrb(acb);
				/* clear Qbuffer if door bell ringed */
				outbound_doorbell = readl(&reg->outbound_doorbell);
				writel(outbound_doorbell, &reg->outbound_doorbell); /*clear interrupt */
   				writel(ARCMSR_INBOUND_DRIVER_DATA_READ_OK, &reg->inbound_doorbell);
				/* enable outbound Post Queue,outbound doorbell Interrupt */
				arcmsr_enable_outbound_ints(acb, intmask_org);
				atomic_set(&acb->rq_map_token, 16);
				atomic_set(&acb->ante_token_value, 16);
				acb->fw_flag = FW_NORMAL;
				mod_timer(&acb->eternal_timer, jiffies + msecs_to_jiffies(6 * HZ));
				acb->acb_flags &= ~ACB_F_BUS_RESET;
				rtn = SUCCESS;
				printk(KERN_ERR "arcmsr: scsi  bus reset eh returns with success\n");
			} else {
				acb->acb_flags &= ~ACB_F_BUS_RESET;
				atomic_set(&acb->rq_map_token, 16);
				atomic_set(&acb->ante_token_value, 16);
				acb->fw_flag = FW_NORMAL;
				mod_timer(&acb->eternal_timer, jiffies + msecs_to_jiffies(6*HZ));
				rtn = SUCCESS;
			}
			break;
		}
		case ACB_ADAPTER_TYPE_B:{
			acb->acb_flags |= ACB_F_BUS_RESET;
			if (!arcmsr_iop_reset(acb)) {
				acb->acb_flags &= ~ACB_F_BUS_RESET;
				rtn = FAILED;
			} else {
				acb->acb_flags &= ~ACB_F_BUS_RESET;
				atomic_set(&acb->rq_map_token, 16);
				atomic_set(&acb->ante_token_value, 16);
				acb->fw_flag = FW_NORMAL;
				mod_timer(&acb->eternal_timer, jiffies + msecs_to_jiffies(6 * HZ));
				rtn = SUCCESS;
			}
			break;
		}
		case ACB_ADAPTER_TYPE_C:{
			if (acb->acb_flags & ACB_F_BUS_RESET) {
				long timeout;
				printk(KERN_ERR "arcmsr: there is an bus reset eh proceeding.......\n");
				timeout = wait_event_timeout(wait_q, (acb->acb_flags & ACB_F_BUS_RESET) == 0, 220*HZ);
				if (timeout) {
					return SUCCESS;
				}
			}
			acb->acb_flags |= ACB_F_BUS_RESET;
			if (!arcmsr_iop_reset(acb)) {
				struct MessageUnit_C __iomem *reg;
				reg = acb->pmuC;
				arcmsr_hardware_reset(acb);
				acb->acb_flags &= ~ACB_F_IOP_INITED;
sleep:
				ssleep(ARCMSR_SLEEPTIME);
				if ((readl(&reg->host_diagnostic) & 0x04) != 0) {
					printk(KERN_ERR "arcmsr%d: waiting for hw bus reset return, retry=%d\n", acb->host->host_no, retry_count);
					if (retry_count > ARCMSR_RETRYCOUNT) {
						acb->fw_flag = FW_DEADLOCK;
						printk(KERN_ERR "arcmsr%d: waiting for hw bus reset return, RETRY TERMINATED!!\n", acb->host->host_no);
						return FAILED;
					}
					retry_count++;
					goto sleep;
				}
				acb->acb_flags |= ACB_F_IOP_INITED;
				/* disable all outbound interrupt */
				intmask_org = arcmsr_disable_outbound_ints(acb);
				arcmsr_get_firmware_spec(acb);
				arcmsr_start_adapter_bgrb(acb);
				/* clear Qbuffer if door bell ringed */
				arcmsr_clear_doorbell_queue_buffer(acb);
				/* enable outbound Post Queue,outbound doorbell Interrupt */
				arcmsr_enable_outbound_ints(acb, intmask_org);
				atomic_set(&acb->rq_map_token, 16);
				atomic_set(&acb->ante_token_value, 16);
				acb->fw_flag = FW_NORMAL;
				mod_timer(&acb->eternal_timer, jiffies + msecs_to_jiffies(6 * HZ));
				acb->acb_flags &= ~ACB_F_BUS_RESET;
				rtn = SUCCESS;
				printk(KERN_ERR "arcmsr: scsi bus reset eh returns with success\n");
			} else {
				acb->acb_flags &= ~ACB_F_BUS_RESET;
				atomic_set(&acb->rq_map_token, 16);
				atomic_set(&acb->ante_token_value, 16);
				acb->fw_flag = FW_NORMAL;
				mod_timer(&acb->eternal_timer, jiffies + msecs_to_jiffies(6*HZ));
				rtn = SUCCESS;
			}
			break;
		}
		case ACB_ADAPTER_TYPE_D: {
			if (acb->acb_flags & ACB_F_BUS_RESET) {
				long timeout;
				pr_notice("arcmsr: there is an bus reset"
					" eh proceeding.......\n");
				timeout = wait_event_timeout(wait_q, (acb->acb_flags
					& ACB_F_BUS_RESET) == 0, 220 * HZ);
				if (timeout)
					return SUCCESS;
			}
			acb->acb_flags |= ACB_F_BUS_RESET;
			if (!arcmsr_iop_reset(acb)) {
				struct MessageUnit_D *reg;
				reg = acb->pmuD;
				arcmsr_hardware_reset(acb);
				acb->acb_flags &= ~ACB_F_IOP_INITED;
			nap:
				ssleep(ARCMSR_SLEEPTIME);
				if ((readl(reg->sample_at_reset) & 0x80) != 0) {
					pr_err("arcmsr%d: waiting for "
						"hw bus reset return, retry=%d\n",
						acb->host->host_no, retry_count);
					if (retry_count > ARCMSR_RETRYCOUNT) {
						acb->fw_flag = FW_DEADLOCK;
						pr_err("arcmsr%d: waiting for hw bus"
							" reset return, "
							"RETRY TERMINATED!!\n",
							acb->host->host_no);
						return FAILED;
					}
					retry_count++;
					goto nap;
				}
				acb->acb_flags |= ACB_F_IOP_INITED;
				/* disable all outbound interrupt */
				intmask_org = arcmsr_disable_outbound_ints(acb);
				arcmsr_get_firmware_spec(acb);
				arcmsr_start_adapter_bgrb(acb);
				arcmsr_clear_doorbell_queue_buffer(acb);
				arcmsr_enable_outbound_ints(acb, intmask_org);
				atomic_set(&acb->rq_map_token, 16);
				atomic_set(&acb->ante_token_value, 16);
				acb->fw_flag = FW_NORMAL;
				mod_timer(&acb->eternal_timer,
					jiffies + msecs_to_jiffies(6 * HZ));
				acb->acb_flags &= ~ACB_F_BUS_RESET;
				rtn = SUCCESS;
				pr_err("arcmsr: scsi bus reset "
					"eh returns with success\n");
			} else {
				acb->acb_flags &= ~ACB_F_BUS_RESET;
				atomic_set(&acb->rq_map_token, 16);
				atomic_set(&acb->ante_token_value, 16);
				acb->fw_flag = FW_NORMAL;
				mod_timer(&acb->eternal_timer,
					jiffies + msecs_to_jiffies(6 * HZ));
				rtn = SUCCESS;
			}
			break;
		}
	}
	return rtn;
}
