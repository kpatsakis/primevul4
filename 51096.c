static void arcmsr_message_isr_bh_fn(struct work_struct *work) 
{
	struct AdapterControlBlock *acb = container_of(work,
		struct AdapterControlBlock, arcmsr_do_message_isr_bh);
	char *acb_dev_map = (char *)acb->device_map;
	uint32_t __iomem *signature = NULL;
	char __iomem *devicemap = NULL;
	int target, lun;
	struct scsi_device *psdev;
	char diff, temp;

	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_A: {
		struct MessageUnit_A __iomem *reg  = acb->pmuA;

		signature = (uint32_t __iomem *)(&reg->message_rwbuffer[0]);
		devicemap = (char __iomem *)(&reg->message_rwbuffer[21]);
		break;
	}
	case ACB_ADAPTER_TYPE_B: {
		struct MessageUnit_B *reg  = acb->pmuB;

		signature = (uint32_t __iomem *)(&reg->message_rwbuffer[0]);
		devicemap = (char __iomem *)(&reg->message_rwbuffer[21]);
		break;
	}
	case ACB_ADAPTER_TYPE_C: {
		struct MessageUnit_C __iomem *reg  = acb->pmuC;

		signature = (uint32_t __iomem *)(&reg->msgcode_rwbuffer[0]);
		devicemap = (char __iomem *)(&reg->msgcode_rwbuffer[21]);
		break;
	}
	case ACB_ADAPTER_TYPE_D: {
		struct MessageUnit_D *reg  = acb->pmuD;

		signature = (uint32_t __iomem *)(&reg->msgcode_rwbuffer[0]);
		devicemap = (char __iomem *)(&reg->msgcode_rwbuffer[21]);
		break;
	}
	}
	atomic_inc(&acb->rq_map_token);
	if (readl(signature) != ARCMSR_SIGNATURE_GET_CONFIG)
		return;
	for (target = 0; target < ARCMSR_MAX_TARGETID - 1;
		target++) {
		temp = readb(devicemap);
		diff = (*acb_dev_map) ^ temp;
		if (diff != 0) {
			*acb_dev_map = temp;
			for (lun = 0; lun < ARCMSR_MAX_TARGETLUN;
				lun++) {
				if ((diff & 0x01) == 1 &&
					(temp & 0x01) == 1) {
					scsi_add_device(acb->host,
						0, target, lun);
				} else if ((diff & 0x01) == 1
					&& (temp & 0x01) == 0) {
					psdev = scsi_device_lookup(acb->host,
						0, target, lun);
					if (psdev != NULL) {
						scsi_remove_device(psdev);
						scsi_device_put(psdev);
					}
				}
				temp >>= 1;
				diff >>= 1;
			}
		}
		devicemap++;
		acb_dev_map++;
	}
}
