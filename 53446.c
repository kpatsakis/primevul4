static inline void _gdContributionsFree(LineContribType * p)
{
	unsigned int u;
	for (u = 0; u < p->LineLength; u++)  {
		gdFree(p->ContribRow[u].Weights);
	}
	gdFree(p->ContribRow);
	gdFree(p);
}
