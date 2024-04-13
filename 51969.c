MagickExport void GetAffineMatrix(AffineMatrix *affine_matrix)
{
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(affine_matrix != (AffineMatrix *) NULL);
  (void) ResetMagickMemory(affine_matrix,0,sizeof(*affine_matrix));
  affine_matrix->sx=1.0;
  affine_matrix->sy=1.0;
}
