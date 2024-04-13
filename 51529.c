static int ec_device_remove(struct platform_device *pdev)
{
	struct cros_ec_dev *ec = dev_get_drvdata(&pdev->dev);
	cdev_del(&ec->cdev);
	device_unregister(&ec->class_dev);
	return 0;
}
