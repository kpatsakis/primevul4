std::unique_ptr<content::OverlayWindow> content::OverlayWindow::Create(
    content::PictureInPictureWindowController* controller) {
  return base::WrapUnique(new OverlayWindowViews(controller));
}
