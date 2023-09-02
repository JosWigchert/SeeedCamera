#include <WiFi.h> //https://github.com/esp8266/Arduino
// #include <ArduinoOTA.h>
#include <SPIFFS.h>
#include <SPI.h>
#include <SD.h>
#include <SimpleTimer.h>
#include <EEPROM.h>

#include "camera_pins.h"
#include "esp_camera.h"
#include "SimpleWebServer.h"
#include "elements/Elements.h"
#include "Status.h"
#include "SDStatus.h"

#define IMAGE_INTERVAL_ADDRESS 512
#define IMAGE_INTERVAL_SIZE sizeof(int)
#define CAMERA_ORIENTATION_ADDRESS IMAGE_INTERVAL_ADDRESS + IMAGE_INTERVAL_SIZE
#define CAMERA_ORIENTATION_SIZE sizeof(int)
#define CAMERA_FRAMESIZE_ADDRESS CAMERA_ORIENTATION_ADDRESS + CAMERA_ORIENTATION_SIZE
#define CAMERA_FRAMESIZE_SIZE sizeof(int)

// Image orientation with JPEG Exif values
enum class CameraOrientation
{
    ORIENTATION_DEFAULT = 1,
    ORIENTATION_DEFAULT_FLIPPED = 2,
    ORIENTATION_90_DEGREES = 6,
    ORIENTATION_90_DEGREES_FLIPPED = 5,
    ORIENTATION_180_DEGREES = 3,
    ORIENTATION_180_DEGREES_FLIPPED = 4,
    ORIENTATION_270_DEGREES = 8,
    ORIENTATION_270_DEGREES_FLIPPED = 7
};

SimpleWebServer webServer;

int imageInterval = 15;
CameraOrientation cameraOrientation = CameraOrientation::ORIENTATION_DEFAULT;
framesize_t cameraFramesize = FRAMESIZE_UXGA;

SimpleTimer cameraTimer(imageInterval * 1000);
bool cameraTimerRunning = false;
SimpleTimer sdTimer(1 * 1000);
bool sdTimerRunning = false;

String status = "";

sdcard_type_t SDcardType = sdcard_type_t::CARD_NONE;
String SDcardTypeString = SDStatus::CARD_NONE;

String usedSpace = "No Card";

bool camera_sign = false;

int imagesInCurrentRound = 0;

TaskHandle_t captureTaskHandle = NULL;

camera_config_t config;

void (*resetFunc)(void) = 0; // declare reset function @ address 0
void handleResetButton();

void handleButtonPress();
void handleImageIntervalChange(String value);
void handleCameraOrientationChanged(int orientation);
void handleCameraFramesizeChanged(int framesize);

void startCameraTimer();
void cameraThreadTask(void *parameter);
void cameraTimerCallback();
bool writeFile(fs::FS &fs, const char *path, uint8_t *data, size_t len);
void photo_save(const char *fileName);
void setExifOrientation(camera_fb_t *fb, CameraOrientation orientation);
int getLatestImageId();
void stopCameraTimer();

void startSDTimer();
void SDTimerCallback();
void stopSDTimer();

void checkSDStatus();
String formatBytes(uint32_t bytes);

void setup()
{
    // Setup Serial Monitor
    Serial.begin(115200);
    Serial.println("Starting ESP");

    // Initialize SPIFFS to get all the javascript and css files from the filesystem
    if (!SPIFFS.begin())
    {
        Serial.println("Failed to mount file system");
        return;
    }
    Serial.println("File system mounted successfully");

    // Initialize the EEPROM with the default size
    EEPROM.begin(1024);

    Serial.printf("Reading EEPROM ImageInterval ADDRESS=%d SIZE=%d\n", IMAGE_INTERVAL_ADDRESS, IMAGE_INTERVAL_SIZE);
    imageInterval = EEPROM.readInt(IMAGE_INTERVAL_ADDRESS);

    if (imageInterval == 0)
    {
        imageInterval = 15;
        EEPROM.writeInt(IMAGE_INTERVAL_ADDRESS, imageInterval);
    }
    cameraTimer.setInterval(imageInterval);

    Serial.printf("Reading EEPROM CameraOrientation ADDRESS=%d SIZE=%d\n", CAMERA_ORIENTATION_ADDRESS, CAMERA_ORIENTATION_SIZE);
    cameraOrientation = (CameraOrientation)EEPROM.readInt(CAMERA_ORIENTATION_ADDRESS);

    if ((int)cameraOrientation < 1 || (int)cameraOrientation > 8)
    {
        cameraOrientation = CameraOrientation::ORIENTATION_DEFAULT;
        EEPROM.writeInt(IMAGE_INTERVAL_ADDRESS, (int)cameraOrientation);
    }

    Serial.printf("Reading EEPROM CameraFramesize ADDRESS=%d SIZE=%d\n", CAMERA_FRAMESIZE_ADDRESS, CAMERA_FRAMESIZE_SIZE);
    cameraFramesize = (framesize_t)EEPROM.readInt(CAMERA_FRAMESIZE_ADDRESS);

    if ((int)cameraFramesize < 1 || (int)cameraFramesize > 22)
    {
        cameraFramesize = FRAMESIZE_UXGA;
        EEPROM.writeInt(CAMERA_FRAMESIZE_ADDRESS, (int)cameraFramesize);
    }

    // Get the unique chip ID as a string
    String chipID = String((uint32_t)ESP.getEfuseMac(), HEX);

    // Create the SSID with the chip ID
    String ssid = "ESP_" + chipID;

    // Start the access point
    const char *password = ""; // The password for the access point
    WiFi.softAP(ssid.c_str(), password);

    Serial.println("Access Point started");
    Serial.print("SSID: ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP());

    webServer.begin();
    // add elements to the webserver
    webServer.addHTMLElement(Position(0, 0), new TextBlock(4, "current_status", "Current Status"));
    webServer.addHTMLElement(Position(0, 1), new TextBlock(4, "sd_card_status", "SD card status"));
    webServer.addHTMLElement(Position(0, 2), new TextBlock(4, "used_space", "Used Space"));

    webServer.addHTMLElement(Position(1, 0), new TextBlock(4, "image_interval", "Image Interval (s)"));
    webServer.addHTMLElement(Position(1, 1), new TextBlock(4, "images_current_round", "Images in CurrentRound"));

    std::map<int, String> orientations;
    orientations.emplace((int)CameraOrientation::ORIENTATION_DEFAULT, "0 Degrees (Landscape)");
    orientations.emplace((int)CameraOrientation::ORIENTATION_DEFAULT_FLIPPED, "0 Degrees (Landscape Flipped)");
    orientations.emplace((int)CameraOrientation::ORIENTATION_90_DEGREES, "90 Degrees (Portrait)");
    orientations.emplace((int)CameraOrientation::ORIENTATION_90_DEGREES_FLIPPED, "90 Degrees (Portrait Flipped)");
    orientations.emplace((int)CameraOrientation::ORIENTATION_180_DEGREES, "180 Degrees (Landscape)");
    orientations.emplace((int)CameraOrientation::ORIENTATION_180_DEGREES_FLIPPED, "180 Degrees (Landscape Flipped)");
    orientations.emplace((int)CameraOrientation::ORIENTATION_270_DEGREES, "270 Degrees (Portrait)");
    orientations.emplace((int)CameraOrientation::ORIENTATION_270_DEGREES_FLIPPED, "270 Degrees (Portrait Flipped)");

    std::map<int, String> framesizes;
    framesizes.emplace((int)FRAMESIZE_96X96, "Resolution 96x96");
    framesizes.emplace((int)FRAMESIZE_QQVGA, "Resolution 160x120");
    framesizes.emplace((int)FRAMESIZE_QCIF, "Resolution 176x144");
    framesizes.emplace((int)FRAMESIZE_HQVGA, "Resolution 240x176");
    framesizes.emplace((int)FRAMESIZE_240X240, "Resolution 240x240");
    framesizes.emplace((int)FRAMESIZE_QVGA, "Resolution 320x240");
    framesizes.emplace((int)FRAMESIZE_CIF, "Resolution 400x296");
    framesizes.emplace((int)FRAMESIZE_HVGA, "Resolution 480x320");
    framesizes.emplace((int)FRAMESIZE_VGA, "Resolution 640x480");
    framesizes.emplace((int)FRAMESIZE_SVGA, "Resolution 800x600");
    framesizes.emplace((int)FRAMESIZE_XGA, "Resolution 1024x768");
    framesizes.emplace((int)FRAMESIZE_HD, "Resolution 1280x720");
    framesizes.emplace((int)FRAMESIZE_SXGA, "Resolution 1280x1024");
    framesizes.emplace((int)FRAMESIZE_UXGA, "Resolution 1600x1200");
    framesizes.emplace((int)FRAMESIZE_FHD, "Resolution 1920x1080");
    framesizes.emplace((int)FRAMESIZE_P_HD, "Resolution 720x1280 (Portrait)");
    framesizes.emplace((int)FRAMESIZE_P_3MP, "Resolution 864x1536 (Portrait)");
    framesizes.emplace((int)FRAMESIZE_QXGA, "Resolution 2048x1536");
    framesizes.emplace((int)FRAMESIZE_QHD, "Resolution 2560x1440");
    framesizes.emplace((int)FRAMESIZE_WQXGA, "Resolution 2560x1600");
    framesizes.emplace((int)FRAMESIZE_P_FHD, "Resolution 1080x1920 (Portrait)");
    framesizes.emplace((int)FRAMESIZE_QSXGA, "Resolution 2560x1920");

    // Order doesn't matter
    webServer.addHTMLElement(Position(2, 0), new TextInput(4, "image_interval", "Image Interval", "Enter a number", TextInput::InputType::NUMBER, handleImageIntervalChange));
    webServer.addHTMLElement(Position(2, 1), new ComboBox(4, "orientation_options", "Camera Orientations", orientations, (int)cameraOrientation, handleCameraOrientationChanged));
    webServer.addHTMLElement(Position(2, 2), new ComboBox(4, "resolutions", "Resolutions", framesizes, (int)cameraOrientation, handleCameraFramesizeChanged));
    webServer.addHTMLElement(Position(3, 1), new Button(4, "start_stop_button", "Start / Stop Gathering", handleButtonPress));
    webServer.addHTMLElement(Position(3, 2), new Button(4, "reset", "Reset", handleResetButton));

    // Add values to watch when the page asks for data
    webServer.addValueWatch("current_status", &status, ValueType::STRING_TYPE);
    webServer.addValueWatch("used_space", &usedSpace, ValueType::STRING_TYPE);
    webServer.addValueWatch("sd_card_status", &SDcardTypeString, ValueType::STRING_TYPE);
    webServer.addValueWatch("image_interval", &imageInterval, ValueType::INT_TYPE);
    webServer.addValueWatch("images_current_round", &imagesInCurrentRound, ValueType::INT_TYPE);

    status = String(Status::IDLE);

    // Setup SD
    if (!SD.begin(21))
    {
        Serial.println("Card Mount Failed");
        return;
    }

    checkSDStatus();

    startSDTimer();

    // Camera initialisation
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.frame_size = cameraFramesize;
    config.pixel_format = PIXFORMAT_JPEG; // for streaming
    // config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.jpeg_quality = 10;
    config.fb_count = 1;

    // FRAMESIZE_QVGA (320 x 240)
    // FRAMESIZE_CIF (352 x 288)
    // FRAMESIZE_VGA (640 x 480)
    // FRAMESIZE_SVGA (800 x 600)
    // FRAMESIZE_XGA (1024 x 768)
    // FRAMESIZE_SXGA (1280 x 1024)
    // FRAMESIZE_UXGA (1600 x 1200)

    // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
    //                      for larger pre-allocated frame buffer.
    if (config.pixel_format == PIXFORMAT_JPEG)
    {
        if (psramFound())
        {
            config.jpeg_quality = 3;
            config.fb_count = 4;
            config.grab_mode = CAMERA_GRAB_LATEST;
        }
        else
        {
            // Limit the frame size when PSRAM is not available
            config.frame_size = FRAMESIZE_SVGA;
            config.fb_location = CAMERA_FB_IN_DRAM;
        }
    }
    else
    {
        // Best option for face detection/recognition
        config.frame_size = FRAMESIZE_240X240;
#if CONFIG_IDF_TARGET_ESP32S3
        config.fb_count = 2;
#endif
    }

#if defined(CAMERA_MODEL_ESP_EYE)
    pinMode(13, INPUT_PULLUP);
    pinMode(14, INPUT_PULLUP);
#endif

    // camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK)
    {
        Serial.printf("Camera init failed with error 0x%x", err);
    }

    camera_sign = true; // Camera initialization check passes

    xTaskCreatePinnedToCore(
        cameraThreadTask,
        "CaptureTask",
        32768, // Stack size
        NULL,
        1, // Priority
        &captureTaskHandle,
        1 // Core number (1 for the second core)
    );
}

void loop()
{
    // ArduinoOTA.handle();
    webServer.handleClient();

    if (sdTimer.isReady() && sdTimerRunning)
    {
        SDTimerCallback();
        sdTimer.reset();
    }
}

void handleResetButton()
{
    Serial.println("Resetting ESP...\n");
    resetFunc();
    Serial.println("ESP is reset, this should not show :/\n");
}

void handleButtonPress()
{
    if (cameraTimerRunning)
    {
        stopCameraTimer();
    }
    else
    {
        startCameraTimer();
    }
    webServer.pushUpdate();
}

void handleImageIntervalChange(String value)
{
    imageInterval = value.toInt();
    cameraTimer.setInterval(imageInterval);
    EEPROM.writeInt(IMAGE_INTERVAL_ADDRESS, imageInterval);
    EEPROM.commit();

    Serial.printf("Image Interval changed to: %d\n", imageInterval);
    webServer.pushUpdate();
}

void handleCameraOrientationChanged(int orientation)
{
    cameraOrientation = (CameraOrientation)orientation;
    EEPROM.writeInt(CAMERA_ORIENTATION_ADDRESS, (int)cameraOrientation);
    EEPROM.commit();

    switch (cameraOrientation)
    {
    case CameraOrientation::ORIENTATION_DEFAULT:
        Serial.println("Camera Orientation changed to: ORIENTATION_DEFAULT");
        break;
    case CameraOrientation::ORIENTATION_DEFAULT_FLIPPED:
        Serial.println("Camera Orientation changed to: ORIENTATION_DEFAULT_FLIPPED");
        break;
    case CameraOrientation::ORIENTATION_90_DEGREES:
        Serial.println("Camera Orientation changed to: ORIENTATION_90_DEGREES");
        break;
    case CameraOrientation::ORIENTATION_90_DEGREES_FLIPPED:
        Serial.println("Camera Orientation changed to: ORIENTATION_90_DEGREES_FLIPPED");
        break;
    case CameraOrientation::ORIENTATION_180_DEGREES:
        Serial.println("Camera Orientation changed to: ORIENTATION_180_DEGREES");
        break;
    case CameraOrientation::ORIENTATION_180_DEGREES_FLIPPED:
        Serial.println("Camera Orientation changed to: ORIENTATION_180_DEGREES_FLIPPED");
        break;
    case CameraOrientation::ORIENTATION_270_DEGREES:
        Serial.println("Camera Orientation changed to: ORIENTATION_270_DEGREES");
        break;
    case CameraOrientation::ORIENTATION_270_DEGREES_FLIPPED:
        Serial.println("Camera Orientation changed to: ORIENTATION_270_DEGREES_FLIPPED");
        break;
    default:
        break;
    }
}

void handleCameraFramesizeChanged(int framesize)
{
    cameraFramesize = (framesize_t)framesize;
    EEPROM.writeInt(CAMERA_FRAMESIZE_ADDRESS, (int)cameraFramesize);
    EEPROM.commit();

    Serial.printf("Framesize Changed");
    config.frame_size = cameraFramesize;

    esp_err_t err = esp_camera_deinit();
    if (err != ESP_OK)
    {
        Serial.printf("Camera deinit failed with error 0x%x", err);
    }

    err = esp_camera_init(&config);
    if (err != ESP_OK)
    {
        Serial.printf("Camera init failed with error 0x%x", err);
    }
}

void startCameraTimer()
{
    cameraTimer.reset();
    cameraTimerRunning = true;
    imagesInCurrentRound = 0;
    Serial.printf("Started picture timer\n");
    status = String(Status::RUNNING);
}

void cameraThreadTask(void *parameter)
{
    Serial.println("start thread");
    while (1)
    {
        Serial.println("in thread");
        Serial.printf("Camera Running %s\n", cameraTimerRunning ? "true" : "false");
        if (cameraTimerRunning)
        {
            cameraTimerCallback();
            imagesInCurrentRound++;
        }
        vTaskDelay(imageInterval * 1000 / portTICK_PERIOD_MS); // Capture an image every 5 seconds
    }
    Serial.println("end of thread");
}

void cameraTimerCallback()
{
    checkSDStatus();

    if (SDcardType == sdcard_type_t::CARD_NONE)
    {
        Serial.println("No SD card to write to");
        return;
    }

    int id = getLatestImageId();
    photo_save(("/images/image_" + String(id + 1) + ".jpg").c_str());
}

// SD card write file
bool writeFile(fs::FS &fs, const char *path, uint8_t *data, size_t len)
{
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if (!file)
    {
        Serial.println("Failed to open file for writing");
        return false;
    }
    if (file.write(data, len) == len)
    {
        return true;
        Serial.println("File written");
    }
    Serial.println("Not able to write file");
    file.close();
    return false;
}

// Save pictures to SD card
void photo_save(const char *fileName)
{
    // Take a photo
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb)
    {
        Serial.println("Failed to get camera frame buffer");
        return;
    }
    // Save photo to file

    Serial.printf("Frame buffer has %d values with %d pixels\n", fb->len, fb->len / 3);
    // setExifOrientation(fb, cameraOrientation);

    writeFile(SD, fileName, fb->buf, fb->len);

    // Release image buffer
    esp_camera_fb_return(fb);

    Serial.println("Photo saved to file");
}

void setExifOrientation(camera_fb_t *fb, CameraOrientation orientation)
{
    if (fb && fb->format == PIXFORMAT_JPEG)
    {
        Serial.println("Trying the EXIF");
        // Locate the EXIF APP1 segment
        uint8_t *jpegData = fb->buf;
        size_t jpegDataSize = fb->len;

        size_t pos = 0;
        while (pos < jpegDataSize - 4)
        {
            if (jpegData[pos] == 0xFF && jpegData[pos + 1] == 0xE1)
            {
                // Found the APP1 segment
                Serial.println("Found the APP1 segment");
                uint16_t segmentLength = (jpegData[pos + 2] << 8) | jpegData[pos + 3];
                if (pos + segmentLength < jpegDataSize)
                {
                    // Check for "Exif" identifier in the segment
                    Serial.println("Check for \"Exif\" identifier in the segment");
                    Serial.print((char)(jpegData + pos)[0]);
                    Serial.print((char)(jpegData + pos)[1]);
                    Serial.print((char)(jpegData + pos)[2]);
                    Serial.print((char)(jpegData + pos)[3]);
                    Serial.print((char)(jpegData + pos)[4]);
                    Serial.print((char)(jpegData + pos)[5]);
                    Serial.print((char)(jpegData + pos)[6]);
                    Serial.println((char)(jpegData + pos)[7]);

                    if (memcmp(jpegData + pos + 4, "Exif", 4) == 0)
                    {
                        // Modify the orientation tag (ID: 0x0112)
                        Serial.println("Changing Exif");
                        uint16_t *exifOrientation = (uint16_t *)(jpegData + pos + 18);
                        *exifOrientation = (uint16_t)orientation;

                        // Optionally, recalculate the EXIF segment length
                        // and update the segment length field in the segment header

                        // Exit the loop after modifying the EXIF tag
                        break;
                    }
                }
            }
            pos++;
        }
    }
}

int getLatestImageId()
{
    int latestId = 0;
    File root = SD.open("/images");

    if (!root)
    {
        Serial.println("Failed to open directory.");
        return latestId;
    }

    while (true)
    {
        File entry = root.openNextFile();
        if (!entry)
        {
            break; // No more files
        }

        String filename = entry.name();
        entry.close();

        if (filename.startsWith("image_") && filename.endsWith(".jpg"))
        {
            int id = filename.substring(6, filename.lastIndexOf(".")).toInt();
            if (id > latestId)
            {
                latestId = id;
            }
        }
    }

    root.close();
    return latestId;
}

void stopCameraTimer()
{
    cameraTimerRunning = false;
    Serial.printf("Stopped picture timer\n");
    status = String(Status::IDLE);
}

void startSDTimer()
{
    sdTimer.reset();
    sdTimerRunning = true;
    Serial.printf("Started SD timer\n");
}

void SDTimerCallback()
{
    checkSDStatus();
}

void stopSDTimer()
{
    sdTimerRunning = false;
    Serial.printf("Stopped SD timer\n");
}

void checkSDStatus()
{
    SDcardType = SD.cardType();

    // Determine if the type of SD card is available
    if (SDcardType == CARD_NONE)
    {
        // Serial.println("No SD card attached");
        SDcardTypeString = SDStatus::CARD_NONE;
        // usedSpace = "No Card";
        return;
    }

    // Get the total and free space on the SD card
    uint32_t totalSpaceBytes = SD.totalBytes();
    uint32_t usedSpaceBytes = SD.usedBytes();

    usedSpace = formatBytes(usedSpaceBytes) + " out of " + formatBytes(totalSpaceBytes) + " used";

    // Serial.print("SD Card Type: ");
    if (SDcardType == CARD_MMC)
    {
        // Serial.println("MMC");
        SDcardTypeString = SDStatus::CARD_MMC;
    }
    else if (SDcardType == CARD_SD)
    {
        // Serial.println("SDSC");
        SDcardTypeString = SDStatus::CARD_SD;
    }
    else if (SDcardType == CARD_SDHC)
    {
        // Serial.println("SDHC");
        SDcardTypeString = SDStatus::CARD_SDHC;
    }
    else
    {
        // Serial.println("UNKNOWN");
        SDcardTypeString = SDStatus::UNKNOWN;
    }
}

String formatBytes(uint32_t bytes)
{
    if (bytes < 1024)
    {
        return String(bytes) + " B";
    }
    else if (bytes < 1024 * 1024)
    {
        return String(bytes / 1024.0) + " KB";
    }
    else if (bytes < 1024 * 1024 * 1024)
    {
        return String(bytes / (1024.0 * 1024.0)) + " MB";
    }
    else
    {
        return String(bytes / (1024.0 * 1024.0 * 1024.0)) + " GB";
    }
}