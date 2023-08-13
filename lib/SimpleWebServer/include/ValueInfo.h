#pragma once

enum ValueType
{
    INT_TYPE,
    FLOAT_TYPE,
    DOUBLE_TYPE,
    BOOL_TYPE,
    STRING_TYPE,
    STD_STRING_TYPE
};

struct ValueInfo
{
    void *valuePtr;
    ValueType valueType;
};