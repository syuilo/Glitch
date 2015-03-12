#include "Glitch.h"

typedef struct {
	unsigned long	index;
	char			str[256];
} TableString;



TableString		g_strs[StrID_NUMTYPES] = {
	StrID_NONE,						"",
	StrID_Name,						"Glitch",
	StrID_Description,				"\rCopyright (c) 2014-2015 syuilo All rights reserved.\rhttp://syuilo.com\r\r�����Ђ܁E�Ђ܂�����������",
	StrID_Err_LoadSuite,			"Error loading suite.",
	StrID_Err_FreeSuite,			"Error releasing suite.",
};


char	*GetStringPtr(int strNum)
{
	return g_strs[strNum].str;
}
	