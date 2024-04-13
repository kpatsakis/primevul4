const string& DeviceNameOrUnspecified(Device* device) {
  static string* unspecified_string = new string("<unspecified>");
  return (device == nullptr) ? *unspecified_string : device->name();
}