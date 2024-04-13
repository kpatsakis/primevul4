gfx::Rect OverlayWindowViews::CalculateAndUpdateWindowBounds() {
  gfx::Rect work_area =
      display::Screen::GetScreen()
          ->GetDisplayNearestWindow(
              controller_->GetInitiatorWebContents()->GetTopLevelNativeWindow())
          .work_area();

  max_size_ = gfx::Size(work_area.width() / 2, work_area.height() / 2);

  min_size_ = kMinWindowSize;

  gfx::Size window_size = window_bounds_.size();
  if (!has_been_shown_) {
    window_size = gfx::Size(work_area.width() / 5, work_area.height() / 5);
    window_size.set_width(std::min(
        max_size_.width(), std::max(min_size_.width(), window_size.width())));
    window_size.set_height(
        std::min(max_size_.height(),
                 std::max(min_size_.height(), window_size.height())));
  }

  if (!window_size.IsEmpty() && !natural_size_.IsEmpty()) {
    float aspect_ratio = (float)natural_size_.width() / natural_size_.height();

    gfx::Rect window_rect(GetBounds().origin(), window_size);
    views::WindowResizeUtils::SizeRectToAspectRatio(
        views::HitTest::kBottomRight, aspect_ratio, min_size_, max_size_,
        &window_rect);
    window_size.SetSize(window_rect.width(), window_rect.height());

    UpdateLayerBoundsWithLetterboxing(window_size);
  }

  gfx::Point origin = window_bounds_.origin();

  int window_diff_width = work_area.right() - window_size.width();
  int window_diff_height = work_area.bottom() - window_size.height();

  int buffer = (window_diff_width + window_diff_height) / 2 * 0.02;

  gfx::Point default_origin =
      gfx::Point(window_diff_width - buffer, window_diff_height - buffer);

  if (has_been_shown_) {
    origin.SetToMin(default_origin);
  } else {
    origin = default_origin;
  }

  window_bounds_ = gfx::Rect(origin, window_size);
  return window_bounds_;
}
