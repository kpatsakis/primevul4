  void NotifyCellularDataPlanChanged() {
    FOR_EACH_OBSERVER(CellularDataPlanObserver,
                      data_plan_observers_,
                      OnCellularDataPlanChanged(this));
  }
