#pragma once

#include "includes.h"

enum stopCodes {genericErr,featureNotFound,regionInvalid,menuIndexInvalid};


/// @brief Shows a BSOD.
/// @param stopcode The error code.
void killExec(u32 stopcode);