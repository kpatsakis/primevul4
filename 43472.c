void cgsem_destroy(cgsem_t *cgsem)
{
	close(cgsem->pipefd[1]);
	close(cgsem->pipefd[0]);
}
