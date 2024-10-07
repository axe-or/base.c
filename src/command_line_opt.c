#include "./command_line_opt.h"

static inline
bool _is_whitespace(char c){
	return c == ' ' || c == '\t' || c == '\v' || c == '\r' || c == '\n';
}

static inline
bool _is_numeric(char c){
	return c >= '0' && c <= '9';
}

CLI_Arg cli_parse_arg(String arg){
	String s = str_trim(arg, str_from(" \t\v\r\n"));
	CLI_Arg res = {
		.value = str_from(""),
		.key = str_from(""),
		.type = CLI_Flag_None,
	};

	if(s.len > 1){
		bool is_flag = s.data[0] == '-' &&
			!_is_whitespace(s.data[1]) &&
			!_is_numeric(s.data[1]) &&
			s.data[1] != ':';

		if(is_flag){
			res.type = CLI_Flag_Toggle;
			isize colon_pos = -1;

			for(isize i = 1; i < s.len; i ++){
				if(s.data[i] == ':'){
					colon_pos = i;
					break;
				}
			}

			isize keylen = max(colon_pos, 1);
			res.key = str_sub(s, 1, keylen);
			if(colon_pos > 0){
				res.value = str_sub(s, colon_pos + 1, s.len - colon_pos - 1);
				res.type = CLI_Flag_Key_Value;
			}
		}
		else {
			res.value = s;
		}
	}
	else {
		res.value = s;
	}
	return res;
}