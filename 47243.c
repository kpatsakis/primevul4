void CLASS foveon_make_curves
	(short **curvep, float dq[3], float div[3], float filt)
{
  double mul[3], max=0;
  int c;

  FORC3 mul[c] = dq[c]/div[c];
  FORC3 if (max < mul[c]) max = mul[c];
  FORC3 curvep[c] = foveon_make_curve (max, mul[c], filt);
}
