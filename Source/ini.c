#ifndef _INI_C_
#define _INI_C_

#include <acknex.h>
#include <windows.h>

BOOL WINAPI PathIsRelative(char *path);
#define PRAGMA_API PathIsRelative;Shlwapi!PathIsRelativeA 

function make_path(STRING *path, STRING *filename)
{
	if(PathIsRelative(_chr(filename)))
	{
		str_cpy(path, work_dir);
		str_cat(path, "\\");
		str_cat(path, filename);
	}
	else
	{
		str_cpy(path, filename);
	}
}

int ini_read_int(STRING *inifile, STRING *section, STRING *key, int defaultValue)
{
	char buffer[256];
	GetPrivateProfileString(_chr(section), _chr(key), _chr(str_for_int(NULL, defaultValue)), buffer, 256, _chr(inifile));	
	return str_to_int(buffer);
}

float ini_read_float(STRING *inifile, STRING *section, STRING *key, float defaultValue)
{
	char buffer[256];
	GetPrivateProfileString(_chr(section), _chr(key), _chr(str_for_float(NULL, defaultValue)), buffer, 256, _chr(inifile));	
	return str_to_float(buffer);
}

void ini_read_buffer(STRING *inifile, STRING *section, STRING *key, STRING *defaultValue, char *buffer, int len)
{
	GetPrivateProfileString(_chr(section), _chr(key), _chr(defaultValue), buffer, len, _chr(inifile));	
}

void ini_write_int(STRING *inifile, STRING *section, STRING *key, int value)
{
	WritePrivateProfileString(_chr(section), _chr(key), _chr(str_for_int(NULL, value)), _chr(inifile));
}

void ini_write_float(STRING *inifile, STRING *section, STRING *key, float value)
{
	WritePrivateProfileString(_chr(section), _chr(key), _chr(str_for_float(NULL, value)), _chr(inifile));
}

void ini_write_string(STRING *inifile, STRING *section, STRING *key, STRING *value)
{
	WritePrivateProfileString(_chr(section), _chr(key), _chr(value), _chr(inifile));
}

#endif