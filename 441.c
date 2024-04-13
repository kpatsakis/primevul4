void OverlayWindowViews::TogglePlayPause() {
  bool is_active = controller_->TogglePlayPause();
  play_pause_controls_view_->SetToggled(is_active);
}
