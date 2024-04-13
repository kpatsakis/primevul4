arcDeterminant3x3(double *m)
{
  /* This had better be a 3x3 matrix or we'll fall to bits */
  return m[0] * ( m[4] * m[8] - m[7] * m[5] ) -
         m[3] * ( m[1] * m[8] - m[7] * m[2] ) +
         m[6] * ( m[1] * m[5] - m[4] * m[2] );
}
