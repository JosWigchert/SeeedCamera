#pragma once

class Status
{
public:
    static const char *IDLE;
    static const char *SD_ERROR;
    static const char *RUNNING;
};

const char *Status::IDLE = "Idle";
const char *Status::SD_ERROR = "SD card error";
const char *Status::RUNNING = "Running";
