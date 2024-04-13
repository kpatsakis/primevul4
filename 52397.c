static void ims_pcu_destroy_gamepad(struct ims_pcu *pcu)
{
	struct ims_pcu_gamepad *gamepad = pcu->gamepad;

	input_unregister_device(gamepad->input);
	kfree(gamepad);
}
