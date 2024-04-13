string EscapeOrigName(const string& orig_name) {
  // Replace _ with __ in the original name to avoid name conflicts.
  return absl::StrReplaceAll(orig_name, {{"_", "__"}});
}