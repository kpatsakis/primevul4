void OverlayWindowViews::SetPlaybackState(PlaybackState playback_state) {
  bool controls_parent_layer_visible = GetControlsParentLayer()->visible();

  playback_state_ = playback_state;

  switch (playback_state_) {
    case kPlaying:
      play_pause_controls_view_->SetToggled(true);
      controls_parent_view_->SetVisible(true);
      video_view_->SetVisible(true);
      GetControlsParentLayer()->SetVisible(controls_parent_layer_visible);
      break;
    case kPaused:
      play_pause_controls_view_->SetToggled(false);
      controls_parent_view_->SetVisible(true);
      video_view_->SetVisible(true);
      GetControlsParentLayer()->SetVisible(controls_parent_layer_visible);
      break;
    case kNoVideo:
      controls_scrim_view_->SetVisible(false);
      controls_parent_view_->SetVisible(false);
      video_view_->SetVisible(false);
      GetControlsParentLayer()->SetVisible(false);
      break;
  }
}
