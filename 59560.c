static cmsHTRANSFORM *AcquireTransformThreadSet(Image *image,
  const cmsHPROFILE source_profile,const cmsUInt32Number source_type,
  const cmsHPROFILE target_profile,const cmsUInt32Number target_type,
  const int intent,const cmsUInt32Number flags)
{
  cmsHTRANSFORM
    *transform;

  register ssize_t
    i;

  size_t
    number_threads;

  number_threads=(size_t) GetMagickResourceLimit(ThreadResource);
  transform=(cmsHTRANSFORM *) AcquireQuantumMemory(number_threads,
    sizeof(*transform));
  if (transform == (cmsHTRANSFORM *) NULL)
    return((cmsHTRANSFORM *) NULL);
  (void) ResetMagickMemory(transform,0,number_threads*sizeof(*transform));
  for (i=0; i < (ssize_t) number_threads; i++)
  {
    transform[i]=cmsCreateTransformTHR((cmsContext) image,source_profile,
      source_type,target_profile,target_type,intent,flags);
    if (transform[i] == (cmsHTRANSFORM) NULL)
      return(DestroyTransformThreadSet(transform));
  }
  return(transform);
}
