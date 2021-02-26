# OpenCV_CameraList_MSMF

A simple function to get a list of cameras with a friendly name, id, and path.
To use opencv (cv::CAP_MSMF)

The ID can be used to initialize the camera you want

Short example:

```
msmf::DeviceEnumerator de;
std::map<int, msmf::Device> devices;
devices = de.getVideoDevicesMap();

for (auto const& device : devices) {
int Id = device.first;
std::string Name = device.second.deviceName
std::string Path = device.second.devicePath;
}
```
My version of the code is a modification of the code for DShow:
https://github.com/studiosi/OpenCVDeviceEnumerator
