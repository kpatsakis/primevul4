  static void DataPlanUpdateHandler(void* object,
                                    const char* modem_service_path,
                                    const CellularDataPlanList* dataplan) {
    NetworkLibraryImpl* networklib = static_cast<NetworkLibraryImpl*>(object);
    if (!networklib || !networklib->cellular_network()) {
      return;
    }
    if (networklib->cellular_network()->service_path()
        .compare(modem_service_path) == 0) {
      if (dataplan != NULL) {
        networklib->UpdateCellularDataPlan(dataplan);
      }
    }
  }
