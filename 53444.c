static inline LineContribType * _gdContributionsAlloc(unsigned int line_length, unsigned int windows_size)
{
	unsigned int u = 0;
	LineContribType *res;

	res = (LineContribType *) gdMalloc(sizeof(LineContribType));
	if (!res) {
		return NULL;
	}
	res->WindowSize = windows_size;
	res->LineLength = line_length;
	res->ContribRow = (ContributionType *) gdMalloc(line_length * sizeof(ContributionType));

	for (u = 0 ; u < line_length ; u++) {
		res->ContribRow[u].Weights = (double *) gdMalloc(windows_size * sizeof(double));
	}
	return res;
}
