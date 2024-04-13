void OverlayWindowViews::UpdateControlsVisibility(bool is_visible) {
  if (always_hide_play_pause_button_ && is_visible)
    play_pause_controls_view_->SetVisible(false);

  GetCloseControlsLayer()->SetVisible(is_visible);

#if defined(OS_CHROMEOS)
  GetResizeHandleLayer()->SetVisible(is_visible);
#endif

  GetControlsScrimLayer()->SetVisible(
      (playback_state_ == kNoVideo) ? false : is_visible);
  GetControlsParentLayer()->SetVisible(
      (playback_state_ == kNoVideo) ? false : is_visible);
}
