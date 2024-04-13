cmsBool  _cmsRegisterOptimizationPlugin(cmsContext id, cmsPluginBase* Data)
{
    cmsPluginOptimization* Plugin = (cmsPluginOptimization*) Data;
    _cmsOptimizationCollection* fl;

    if (Data == NULL) {

        OptimizationCollection = DefaultOptimization;
        return TRUE;
    }

    if (Plugin ->OptimizePtr == NULL) return FALSE;

    fl = (_cmsOptimizationCollection*) _cmsPluginMalloc(id, sizeof(_cmsOptimizationCollection));
    if (fl == NULL) return FALSE;

    fl ->OptimizePtr = Plugin ->OptimizePtr;

    fl ->Next = OptimizationCollection;
    OptimizationCollection = fl;

    return TRUE;
}
