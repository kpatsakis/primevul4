static void arcmsr_report_ccb_state(struct AdapterControlBlock *acb, 
			struct CommandControlBlock *ccb, bool error)
{
	uint8_t id, lun;
	id = ccb->pcmd->device->id;
	lun = ccb->pcmd->device->lun;
	if (!error) {
		if (acb->devstate[id][lun] == ARECA_RAID_GONE)
			acb->devstate[id][lun] = ARECA_RAID_GOOD;
		ccb->pcmd->result = DID_OK << 16;
		arcmsr_ccb_complete(ccb);
	}else{
		switch (ccb->arcmsr_cdb.DeviceStatus) {
		case ARCMSR_DEV_SELECT_TIMEOUT: {
			acb->devstate[id][lun] = ARECA_RAID_GONE;
			ccb->pcmd->result = DID_NO_CONNECT << 16;
			arcmsr_ccb_complete(ccb);
			}
			break;

		case ARCMSR_DEV_ABORTED:

		case ARCMSR_DEV_INIT_FAIL: {
			acb->devstate[id][lun] = ARECA_RAID_GONE;
			ccb->pcmd->result = DID_BAD_TARGET << 16;
			arcmsr_ccb_complete(ccb);
			}
			break;

		case ARCMSR_DEV_CHECK_CONDITION: {
			acb->devstate[id][lun] = ARECA_RAID_GOOD;
			arcmsr_report_sense_info(ccb);
			arcmsr_ccb_complete(ccb);
			}
			break;

		default:
			printk(KERN_NOTICE
				"arcmsr%d: scsi id = %d lun = %d isr get command error done, \
				but got unknown DeviceStatus = 0x%x \n"
				, acb->host->host_no
				, id
				, lun
				, ccb->arcmsr_cdb.DeviceStatus);
				acb->devstate[id][lun] = ARECA_RAID_GONE;
				ccb->pcmd->result = DID_NO_CONNECT << 16;
				arcmsr_ccb_complete(ccb);
			break;
		}
	}
}
