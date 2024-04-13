static irqreturn_t _vop_virtio_intr_handler(int irq, void *data)
{
	struct vop_vdev *vdev = data;
	struct vop_device *vpdev = vdev->vpdev;

	vpdev->hw_ops->ack_interrupt(vpdev, vdev->virtio_db);
	schedule_work(&vdev->virtio_bh_work);
	return IRQ_HANDLED;
}
