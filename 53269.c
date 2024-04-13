static void release_task_mempolicy(struct proc_maps_private *priv)
{
	mpol_put(priv->task_mempolicy);
}
