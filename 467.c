  virtual CellularNetwork* FindCellularNetworkByPath(
      const std::string& path) {
    return GetWirelessNetworkByPath(cellular_networks_, path);
  }
