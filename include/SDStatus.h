#pragma once

class SDStatus
{
public:
    static const char *MOUNT_FAILED;
    static const char *CARD_NONE;
    static const char *CARD_MMC;
    static const char *CARD_SD;
    static const char *CARD_SDHC;
    static const char *UNKNOWN;
};

const char *SDStatus::MOUNT_FAILED = "Card Mount Failed";
const char *SDStatus::CARD_NONE = "No SD card attached";
const char *SDStatus::CARD_MMC = "Card type MMC";
const char *SDStatus::CARD_SD = "Card type SDSC";
const char *SDStatus::CARD_SDHC = "Card type SDHC";
const char *SDStatus::UNKNOWN = "Card type UNKNOWN";
