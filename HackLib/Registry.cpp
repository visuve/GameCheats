#include "HackLib-PCH.hpp"
#include "Registry.hpp"

Registry::Registry(HKEY parent, std::wstring_view subKey, REGSAM access)
{
	LSTATUS result = RegOpenKeyExW(
		parent,
		subKey.data(),
		REG_NONE,
		access,
		&_key);

	if (result != ERROR_SUCCESS)
	{
		throw Win32ExceptionEx("RegOpenKeyExW", result);
	}
}

Registry::~Registry()
{
	if (_key && !IsPredefined())
	{
		RegCloseKey(_key);
	}
}

bool Registry::IsPredefined() const
{
	// https://docs.microsoft.com/en-us/windows/win32/sysinfo/predefined-keys
	return _key == HKEY_CLASSES_ROOT ||
		_key == HKEY_CURRENT_CONFIG ||
		_key == HKEY_CURRENT_USER ||
		_key == HKEY_CURRENT_USER_LOCAL_SETTINGS ||
		_key == HKEY_LOCAL_MACHINE ||
		_key == HKEY_PERFORMANCE_DATA ||
		_key == HKEY_PERFORMANCE_NLSTEXT ||
		_key == HKEY_PERFORMANCE_TEXT ||
		_key == HKEY_USERS;
}