#include <ntdll.h>
#include <windows.h>

extern "C" {

int __cdecl sscanf(
  _In_z_                       char const* const buffer,
  _In_z_ _Scanf_format_string_ char const* const format,
  ...
  );

int __cdecl printf(
  _In_z_ _Printf_format_string_ const char* const format,
  ...
  );


int __cdecl swscanf(
  _In_z_                       wchar_t const* const buffer,
  _In_z_ _Scanf_format_string_ wchar_t const* const format,
  ...
  );

int __cdecl wprintf(
  _In_z_ _Printf_format_string_ const wchar_t* const format,
  ...
  );


}

int main()
{
  wprintf("hello world!\n");
}
