#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H

#include "esp_camera.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"

struct ImageData {
  uint8_t* data;
  size_t size;
};

class CameraController {
public:
  CameraController();
  bool begin();
  void setImageSize(size_t width, size_t height);
  void capture();
  bool isCaptureAvailable();
  ImageData scaleImage(size_t width, size_t height);

private:
  camera_config_t cameraConfig;
  camera_fb_t* capturedImage;
  size_t imageWidth;
  size_t imageHeight;
};

#endif
