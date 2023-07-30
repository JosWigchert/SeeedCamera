#include "CameraController.h"
#include "camera_pins.h"


CameraController::CameraController()
{
  capturedImage = nullptr;
  imageWidth = 0;
  imageHeight = 0;
}

bool CameraController::begin()
{
  // Configure camera settings

  cameraConfig.ledc_channel = LEDC_CHANNEL_0;
  cameraConfig.ledc_timer = LEDC_TIMER_0;
  cameraConfig.pin_d0 = Y2_GPIO_NUM;
  cameraConfig.pin_d1 = Y3_GPIO_NUM;
  cameraConfig.pin_d2 = Y4_GPIO_NUM;
  cameraConfig.pin_d3 = Y5_GPIO_NUM;
  cameraConfig.pin_d4 = Y6_GPIO_NUM;
  cameraConfig.pin_d5 = Y7_GPIO_NUM;
  cameraConfig.pin_d6 = Y8_GPIO_NUM;
  cameraConfig.pin_d7 = Y9_GPIO_NUM;
  cameraConfig.pin_xclk = XCLK_GPIO_NUM;
  cameraConfig.pin_pclk = PCLK_GPIO_NUM;
  cameraConfig.pin_vsync = VSYNC_GPIO_NUM;
  cameraConfig.pin_href = HREF_GPIO_NUM;
  cameraConfig.pin_sccb_sda = SIOD_GPIO_NUM;
  cameraConfig.pin_sccb_scl = SIOC_GPIO_NUM;
  cameraConfig.pin_pwdn = PWDN_GPIO_NUM;
  cameraConfig.pin_reset = RESET_GPIO_NUM;
  cameraConfig.xclk_freq_hz = 20000000;
  cameraConfig.frame_size = FRAMESIZE_SVGA;
  cameraConfig.pixel_format = PIXFORMAT_JPEG; // for streaming
  //cameraConfig.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
  cameraConfig.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  cameraConfig.fb_location = CAMERA_FB_IN_PSRAM;
  cameraConfig.jpeg_quality = 12;
  cameraConfig.fb_count = 1;

  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if (cameraConfig.pixel_format == PIXFORMAT_JPEG)
  {
    if (psramFound())
    {
      cameraConfig.jpeg_quality = 10;
      cameraConfig.fb_count = 2;
      cameraConfig.grab_mode = CAMERA_GRAB_LATEST;
    }
    else
    {
      // Limit the frame size when PSRAM is not available
      cameraConfig.frame_size = FRAMESIZE_SVGA;
      cameraConfig.fb_location = CAMERA_FB_IN_DRAM;
    }
  }
  else
  {
    // Best option for face detection/recognition
    cameraConfig.frame_size = FRAMESIZE_240X240;
#if CONFIG_IDF_TARGET_ESP32S3
    cameraConfig.fb_count = 2;
#endif
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  // Initialize camera
  esp_err_t err = esp_camera_init(&cameraConfig);
  return err == ESP_OK;
}

void CameraController::setImageSize(size_t width, size_t height)
{
  imageWidth = width;
  imageHeight = height;
}

void CameraController::capture()
{
  if (capturedImage != nullptr)
  {
    esp_camera_fb_return(capturedImage);
    capturedImage = nullptr;
  }

  capturedImage = esp_camera_fb_get();
}

bool CameraController::isCaptureAvailable()
{
  return capturedImage != nullptr;
}

ImageData CameraController::scaleImage(size_t width, size_t height)
{
  if (!isCaptureAvailable())
  {
    return {nullptr, 0};
  }

  size_t imageSize = width * height * 3;
  uint8_t *scaledData = new uint8_t[imageSize];

  // Scale the image using bilinear interpolation
  for (size_t y = 0; y < height; y++)
  {
    for (size_t x = 0; x < width; x++)
    {
      size_t srcX = x * capturedImage->width / width;
      size_t srcY = y * capturedImage->height / height;
      size_t srcIndex = 3 * (srcY * capturedImage->width + srcX);
      size_t dstIndex = 3 * (y * width + x);

      scaledData[dstIndex] = capturedImage->buf[srcIndex];
      scaledData[dstIndex + 1] = capturedImage->buf[srcIndex + 1];
      scaledData[dstIndex + 2] = capturedImage->buf[srcIndex + 2];
    }
  }

  return {scaledData, imageSize};
}
