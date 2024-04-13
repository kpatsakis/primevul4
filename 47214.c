void CLASS blend_highlights()
{
  int clip=INT_MAX, row, col, c, i, j;
  static const float trans[2][4][4] =
  { { { 1,1,1 }, { 1.7320508,-1.7320508,0 }, { -1,-1,2 } },
    { { 1,1,1,1 }, { 1,-1,1,-1 }, { 1,1,-1,-1 }, { 1,-1,-1,1 } } };
  static const float itrans[2][4][4] =
  { { { 1,0.8660254,-0.5 }, { 1,-0.8660254,-0.5 }, { 1,0,1 } },
    { { 1,1,1,1 }, { 1,-1,1,-1 }, { 1,1,-1,-1 }, { 1,-1,-1,1 } } };
  float cam[2][4], lab[2][4], sum[2], chratio;

  if ((unsigned) (colors-3) > 1) return;
  dcraw_message (DCRAW_VERBOSE,_("Blending highlights...\n"));
  FORCC if (clip > (i = 65535*pre_mul[c])) clip = i;
  for (row=0; row < height; row++)
    for (col=0; col < width; col++) {
      FORCC if (image[row*width+col][c] > clip) break;
      if (c == colors) continue;
      FORCC {
	cam[0][c] = image[row*width+col][c];
	cam[1][c] = MIN(cam[0][c],clip);
      }
      for (i=0; i < 2; i++) {
	FORCC for (lab[i][c]=j=0; j < colors; j++)
	  lab[i][c] += trans[colors-3][c][j] * cam[i][j];
	for (sum[i]=0,c=1; c < colors; c++)
	  sum[i] += SQR(lab[i][c]);
      }
      chratio = sqrt(sum[1]/sum[0]);
      for (c=1; c < colors; c++)
	lab[0][c] *= chratio;
      FORCC for (cam[0][c]=j=0; j < colors; j++)
	cam[0][c] += itrans[colors-3][c][j] * lab[0][j];
      FORCC image[row*width+col][c] = cam[0][c] / colors;
    }
}
