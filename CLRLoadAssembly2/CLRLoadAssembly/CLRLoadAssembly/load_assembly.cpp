#include "load_assembly.h"
#include <Windows.h>
#include <metahost.h>

bool load_assembly(LPVOID code, size_t len)
{
	ICorRuntimeHost* icrh = (ICorRuntimeHost*)0;
	IUnknownPtr iu = (IUnknownPtr)0;
	mscorlib::_AppDomainPtr ad = (mscorlib::_AppDomainPtr)0;
	SAFEARRAY* sa = (SAFEARRAY*)0;
	SAFEARRAYBOUND sab;
	mscorlib::_AssemblyPtr Assembly = (mscorlib::_AssemblyPtr)0;
	mscorlib::_MethodInfoPtr mi = (mscorlib::_MethodInfoPtr)0;
	VARIANT v1, v2;
	HRESULT hr;
	typedef HRESULT(__stdcall* _CLRCreateInstance)(REFCLSID clsid, REFIID riid, /*iid_is(riid)*/ LPVOID* ppInterface);

	_CLRCreateInstance ___CLRCreateInstance = (_CLRCreateInstance)GetProcAddress(LoadLibrary(L"mscoree.dll"), "CLRCreateInstance");

	if (___CLRCreateInstance == NULL) {
		hr = CorBindToRuntime(NULL, NULL, CLSID_CorRuntimeHost, IID_ICorRuntimeHost, (LPVOID*)&icrh);
	}
	else {
		ICLRMetaHost* pMetaHost = NULL;
		___CLRCreateInstance(CLSID_CLRMetaHost, IID_ICLRMetaHost, (LPVOID*)&pMetaHost);

		ICLRRuntimeInfo* pRuntimeInfo = NULL;
		pMetaHost->GetRuntime(L"v4.0.30319", IID_ICLRRuntimeInfo, (LPVOID*)&pRuntimeInfo);

		hr = pRuntimeInfo->GetInterface(CLSID_CorRuntimeHost, IID_ICorRuntimeHost, (LPVOID*)&icrh);
	}

	if FAILED(hr) return false;

	hr = icrh->Start();

	if FAILED(hr) return false;

	hr = icrh->GetDefaultDomain(&iu);

	if FAILED(hr) return false;

	hr = iu->QueryInterface(IID_PPV_ARGS(&ad));

	if FAILED(hr) return false;

	sab.cElements = len;
	sab.lLbound = 0;

	sa = SafeArrayCreate(VT_UI1, 1, &sab);

	if (sa == NULL) return false;

	CopyMemory(sa->pvData, code, len);

	hr = ad->Load_3(sa, &Assembly);

	if FAILED(hr) return false;

	hr = Assembly->get_EntryPoint(&mi);

	if FAILED(hr) return false;

	v1.vt = NULL;
	v1.pllVal = NULL;

	hr = mi->Invoke_3(v1, NULL, &v2);

	if FAILED(hr) return false;

	SafeArrayDestroy(sa);

	return true;
}