static int region_type_cap(struct vfio_pci_device *vdev,
			   struct vfio_info_cap *caps,
			   unsigned int type, unsigned int subtype)
{
	struct vfio_info_cap_header *header;
	struct vfio_region_info_cap_type *cap;

	header = vfio_info_cap_add(caps, sizeof(*cap),
				   VFIO_REGION_INFO_CAP_TYPE, 1);
	if (IS_ERR(header))
		return PTR_ERR(header);

	cap = container_of(header, struct vfio_region_info_cap_type, header);
	cap->type = type;
	cap->subtype = subtype;

	return 0;
}
