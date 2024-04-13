void ComputeBlackPointCompensation(const cmsCIEXYZ* BlackPointIn,
                                   const cmsCIEXYZ* BlackPointOut,
                                   cmsMAT3* m, cmsVEC3* off)
{
  cmsFloat64Number ax, ay, az, bx, by, bz, tx, ty, tz;


   tx = BlackPointIn->X - cmsD50_XYZ()->X;
   ty = BlackPointIn->Y - cmsD50_XYZ()->Y;
   tz = BlackPointIn->Z - cmsD50_XYZ()->Z;

   ax = (BlackPointOut->X - cmsD50_XYZ()->X) / tx;
   ay = (BlackPointOut->Y - cmsD50_XYZ()->Y) / ty;
   az = (BlackPointOut->Z - cmsD50_XYZ()->Z) / tz;

   bx = - cmsD50_XYZ()-> X * (BlackPointOut->X - BlackPointIn->X) / tx;
   by = - cmsD50_XYZ()-> Y * (BlackPointOut->Y - BlackPointIn->Y) / ty;
   bz = - cmsD50_XYZ()-> Z * (BlackPointOut->Z - BlackPointIn->Z) / tz;

   _cmsVEC3init(&m ->v[0], ax, 0,  0);
   _cmsVEC3init(&m ->v[1], 0, ay,  0);
   _cmsVEC3init(&m ->v[2], 0,  0,  az);
   _cmsVEC3init(off, bx, by, bz);

}
