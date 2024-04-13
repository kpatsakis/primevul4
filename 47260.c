void CLASS ifpProgress(unsigned readCount) {
/* We don't use this in Rawstudio */
#if 0
    ifpReadCount += readCount;
    if (ifpSize==0) return;
    unsigned newStepProgress = STEPS * ifpReadCount / ifpSize;
    if (newStepProgress > ifpStepProgress) {
#ifdef DCRAW_NOMAIN
	if (ifpStepProgress)
	    progress(PROGRESS_LOAD, newStepProgress - ifpStepProgress);
	else
	    progress(PROGRESS_LOAD, -STEPS);
#endif
    }
    ifpStepProgress = newStepProgress;
#endif
}
