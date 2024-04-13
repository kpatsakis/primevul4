  virtual void ConnectToCellularNetwork(const CellularNetwork* network) {
    DCHECK(network);
    if (!EnsureCrosLoaded())
      return;
    if (network && ConnectToNetwork(network->service_path().c_str(), NULL)) {
      CellularNetwork* cellular = GetWirelessNetworkByPath(
          cellular_networks_, network->service_path());
      if (cellular) {
        cellular->set_connecting(true);
        cellular_ = cellular;
      }
      NotifyNetworkManagerChanged();
    }
  }
