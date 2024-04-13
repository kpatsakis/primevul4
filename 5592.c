first_submatch(regmmatch_T *rp)
{
    int		submatch;

    for (submatch = 1; ; ++submatch)
    {
	if (rp->startpos[submatch].lnum >= 0)
	    break;
	if (submatch == 9)
	{
	    submatch = 0;
	    break;
	}
    }
    return submatch;
}