static const char *arcmsr_info(struct Scsi_Host *host)
{
	struct AdapterControlBlock *acb =
		(struct AdapterControlBlock *) host->hostdata;
	static char buf[256];
	char *type;
	int raid6 = 1;
	switch (acb->pdev->device) {
	case PCI_DEVICE_ID_ARECA_1110:
	case PCI_DEVICE_ID_ARECA_1200:
	case PCI_DEVICE_ID_ARECA_1202:
	case PCI_DEVICE_ID_ARECA_1210:
		raid6 = 0;
		/*FALLTHRU*/
	case PCI_DEVICE_ID_ARECA_1120:
	case PCI_DEVICE_ID_ARECA_1130:
	case PCI_DEVICE_ID_ARECA_1160:
	case PCI_DEVICE_ID_ARECA_1170:
	case PCI_DEVICE_ID_ARECA_1201:
	case PCI_DEVICE_ID_ARECA_1203:
	case PCI_DEVICE_ID_ARECA_1220:
	case PCI_DEVICE_ID_ARECA_1230:
	case PCI_DEVICE_ID_ARECA_1260:
	case PCI_DEVICE_ID_ARECA_1270:
	case PCI_DEVICE_ID_ARECA_1280:
		type = "SATA";
		break;
	case PCI_DEVICE_ID_ARECA_1214:
	case PCI_DEVICE_ID_ARECA_1380:
	case PCI_DEVICE_ID_ARECA_1381:
	case PCI_DEVICE_ID_ARECA_1680:
	case PCI_DEVICE_ID_ARECA_1681:
	case PCI_DEVICE_ID_ARECA_1880:
		type = "SAS/SATA";
		break;
	default:
		type = "unknown";
		raid6 =	0;
		break;
	}
	sprintf(buf, "Areca %s RAID Controller %s\narcmsr version %s\n",
		type, raid6 ? "(RAID6 capable)" : "", ARCMSR_DRIVER_VERSION);
	return buf;
}
