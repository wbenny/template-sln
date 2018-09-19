#include <ntddk.h>

DRIVER_INITIALIZE DriverEntry;

VOID
NTAPI
DriverUnload(
  _In_ PDRIVER_OBJECT DriverObject
  )
{
  UNREFERENCED_PARAMETER(DriverObject);
}

NTSTATUS
NTAPI
DriverEntry(
  _In_ PDRIVER_OBJECT DriverObject,
  _In_ PUNICODE_STRING RegistryPath
  )
{
  UNREFERENCED_PARAMETER(RegistryPath);

  DriverObject->DriverUnload = &DriverUnload;

  return STATUS_SUCCESS;
}
