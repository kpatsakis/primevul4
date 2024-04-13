OverlayWindowViews::WindowQuadrant GetCurrentWindowQuadrant(
    const gfx::Rect window_bounds,
    content::PictureInPictureWindowController* controller) {
  gfx::Rect work_area =
      display::Screen::GetScreen()
          ->GetDisplayNearestWindow(
              controller->GetInitiatorWebContents()->GetTopLevelNativeWindow())
          .work_area();
  gfx::Point window_center = window_bounds.CenterPoint();

  if (window_center.x() < work_area.width() / 2) {
    return (window_center.y() < work_area.height() / 2)
               ? OverlayWindowViews::WindowQuadrant::kTopLeft
               : OverlayWindowViews::WindowQuadrant::kBottomLeft;
  }
  return (window_center.y() < work_area.height() / 2)
             ? OverlayWindowViews::WindowQuadrant::kTopRight
             : OverlayWindowViews::WindowQuadrant::kBottomRight;
}
