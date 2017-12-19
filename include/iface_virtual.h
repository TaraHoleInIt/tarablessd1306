#ifndef _IFACE_VIRTUAL_H_
#define _IFACE_VIRTUAL_H_

int Virt_DeviceInit( struct SSD1306_Device* DeviceHandle, int Width, int Height );
void Virt_DeviceBlit( struct SSD1306_Device* SrcDevice, struct SSD1306_Device* DstDevice, struct Rect SrcRect, struct Rect DstRect );

#endif
