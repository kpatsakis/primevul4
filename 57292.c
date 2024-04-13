static void vmw_surface_destroy_encode(uint32_t id,
				       void *cmd_space)
{
	struct vmw_surface_destroy *cmd = (struct vmw_surface_destroy *)
		cmd_space;

	cmd->header.id = SVGA_3D_CMD_SURFACE_DESTROY;
	cmd->header.size = sizeof(cmd->body);
	cmd->body.sid = id;
}
