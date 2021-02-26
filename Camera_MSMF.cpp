#include "Camera_MSMF.h"



std::string ToNarrow(const wchar_t* s, char dfault = '?',
    const std::locale& loc = std::locale())
{
    std::ostringstream stm;

    while (*s != L'\0') {
        stm << std::use_facet< std::ctype<wchar_t> >(loc).narrow(*s++, dfault);
    }
    return stm.str();
}

namespace msmf {

    std::map<int, Device> DeviceEnumerator::getVideoDevicesMap() {

        return getDevicesMap(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
    }

    std::map<int, Device> DeviceEnumerator::getAudioDevicesMap() {
        return getDevicesMap(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_GUID);
    }


	// Returns a map of id and devices that can be used
	std::map<int, Device> DeviceEnumerator::getDevicesMap(const GUID deviceClass)
	{
		std::map<int, Device> deviceMap;


		IMFAttributes* pConfig = NULL;
		IMFActivate** ppDevices = NULL;
		UINT32 count = 0;
		HRESULT A = MFCreateAttributes(&pConfig, 1);
		int deviceId = -1;

		if (FAILED(A)) {
			return deviceMap; // Empty deviceMap as an error
		}

		A = pConfig->SetGUID(
			MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
			deviceClass
		);

		// Enumerate the devices,
		if (SUCCEEDED(A))
		{
			A = MFEnumDeviceSources(pConfig, &ppDevices, &count);
		}


		if (SUCCEEDED(A)) {

            for (DWORD i = 0; i < count; i++) {
                HRESULT hr = S_OK;
                WCHAR* szFriendlyName = NULL;
                UINT32 cchName;

                hr = ppDevices[i]->GetAllocatedString(
                    MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME,
                    &szFriendlyName, &cchName);

                std::string deviceName;
                std::string devicePath;

                if (FAILED(hr)) { continue; }
                else deviceName = ConvertWCHhartoMBS(szFriendlyName);
                
                CoTaskMemFree(szFriendlyName);

                if (deviceClass == MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID) {
                    hr = ppDevices[i]->GetAllocatedString(
                        MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK,
                        &szFriendlyName, &cchName);
                }
                else {
                    hr = ppDevices[i]->GetAllocatedString(
                        MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_ENDPOINT_ID,
                        &szFriendlyName, &cchName);
                }
                
                if (FAILED(hr)) { continue; }
                else devicePath = ConvertWCHhartoMBS(szFriendlyName);
                CoTaskMemFree(szFriendlyName);

                // We populate the map
                deviceId++;
                Device currentDevice;
                currentDevice.id = deviceId;
                currentDevice.deviceName = deviceName;
                currentDevice.devicePath = devicePath;
                deviceMap[deviceId] = currentDevice;


            }

            for (DWORD i = 0; i < count; i++)
            {
                ppDevices[i]->Release();
            }
            CoTaskMemFree(ppDevices);
			
		}
		return deviceMap;
	}

    std::string DeviceEnumerator::ConvertBSTRToMBS(BSTR bstr)
    {
        int wslen = ::SysStringLen(bstr);
        return ConvertWCSToMBS((wchar_t*)bstr, wslen);
    }

    std::string DeviceEnumerator::ConvertWCSToMBS(const wchar_t* pstr, long wslen)
    {
        int len = ::WideCharToMultiByte(CP_ACP, 0, pstr, wslen, NULL, 0, NULL, NULL);

        std::string dblstr(len, '\0');
        len = ::WideCharToMultiByte(CP_ACP, 0 /* no flags */,
            pstr, wslen /* not necessary NULL-terminated */,
            &dblstr[0], len,
            NULL, NULL /* no default char */);

        return dblstr;
    }

    std::string DeviceEnumerator::ConvertWCHhartoMBS(BSTR bstr)
    {
        return ToNarrow(bstr);
    }


}




