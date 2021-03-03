#pragma once

#pragma warning(disable: 4996)
#pragma comment(lib, "mscoree.lib")

#import "C:\\Windows\\Microsoft.NET\\Framework\\v4.0.30319\\mscorlib.tlb" raw_interfaces_only	\
	high_property_prefixes("_get", "_put", "_putref")	\
	rename("ReportEvent", "InteropServices_ReportEvent")	\
	rename("or", "InteropServices_or")

bool load_assembly(LPVOID code, size_t len);