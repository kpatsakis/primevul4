static void queue_autoload_drivers(void)
{
	schedule_work(&autoload_work);
}
