#pragma once
#include "string.h"

typedef enum {
	CLI_Flag_None = 0,
	CLI_Flag_Toggle = 1,
	CLI_Flag_Key_Value = 2,
} CLI_Flag_Type;

typedef struct {
	String value;
	String key;
	CLI_Flag_Type type;
} CLI_Arg;

// Parse a CLI argument, accepts flags of the form `-key:val` or just `-flag`
CLI_Arg cli_parse_arg(String arg);
