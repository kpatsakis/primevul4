void CurvesFree(cmsContext ContextID, void* ptr)
{
     Curves16Data* Data = (Curves16Data*) ptr;
     int i;

     for (i=0; i < Data -> nCurves; i++) {

         _cmsFree(ContextID, Data ->Curves[i]);
     }

     _cmsFree(ContextID, Data ->Curves);
     _cmsFree(ContextID, ptr);
}
