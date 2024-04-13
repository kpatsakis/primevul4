string GetFlatName(const string orig_name, int index) {
  return absl::StrCat(EscapeOrigName(orig_name), "_", index);
}