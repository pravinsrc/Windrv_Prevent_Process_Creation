#include <ntddk.h>

/*
==============================================================
Function prototypes
==============================================================
*/

NTSTATUS
DriverEntry(
	_In_ PDRIVER_OBJECT DriverObject,
	_In_ PUNICODE_STRING RegistryPath
);

void
badboyDriverUnload(
	PDRIVER_OBJECT DriverObject
);

void PcreateProcessNotifyRoutineEx(
	PEPROCESS Process,
	HANDLE ProcessId,
	PPS_CREATE_NOTIFY_INFO CreateInfo
);

/*
==============================================================
Function implementations
==============================================================
*/

NTSTATUS
DriverEntry(
	_In_ PDRIVER_OBJECT		DriverObject,
	_In_ PUNICODE_STRING	RegistryPath
)
{
	UNREFERENCED_PARAMETER(RegistryPath);

	NTSTATUS status;

	DriverObject->DriverUnload = badboyDriverUnload;

	status = STATUS_SUCCESS;

	DbgPrint("Driver loaded!\n");

	// https://webcache.googleusercontent.com/search?q=cache:4vxTVzmlrd4J:https://bitnuts.de/articles/blocking_process_creation_using_a_windows_kernel_driver.html+&cd=11&hl=en&ct=clnk&gl=ca
	NTSTATUS result;
	result = PsSetCreateProcessNotifyRoutineEx(PcreateProcessNotifyRoutineEx, FALSE);
	if (STATUS_SUCCESS == result) {
		DbgPrint("Added routine!\n");
	}
	else {
		DbgPrint("Failed to add routine! Error: %i\n", result);
	}

	return status;
}

void
badboyDriverUnload(
	PDRIVER_OBJECT DriverObject
)
{
	UNREFERENCED_PARAMETER(DriverObject);

	if (STATUS_SUCCESS == (PsSetCreateProcessNotifyRoutineEx(PcreateProcessNotifyRoutineEx, TRUE))) {
		DbgPrint("Removed routine!\n");
	}
	else {
		DbgPrint("Failed to remove routine!\n");
	}

	DbgPrint("Driver unloaded!\n");
}

void PcreateProcessNotifyRoutineEx(
	PEPROCESS				Process,
	HANDLE					ProcessId,
	PPS_CREATE_NOTIFY_INFO	CreateInfo
)
{
	UNREFERENCED_PARAMETER(Process);

	/*
	CreateInfo is non-NULL indicates a new process is being created
	*/
	if (CreateInfo) {
		DbgPrint("[Process Create] %i: %wZ\n", ProcessId, CreateInfo->CommandLine);
		CreateInfo->CreationStatus = STATUS_ACCESS_DENIED;
	}
}