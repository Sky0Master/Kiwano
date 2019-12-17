// Copyright (c) 2016-2018 Kiwano - Nomango
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <kiwano/platform/Window.h>
#include <kiwano/core/win32/helper.h>
#include <kiwano/platform/Application.h>

#define WINDOW_FIXED_STYLE		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX
#define WINDOW_RESIZABLE_STYLE	WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_SIZEBOX | WS_MAXIMIZEBOX
#define WINDOW_FULLSCREEN_STYLE	WS_CLIPCHILDREN | WS_POPUP
#define KGE_WND_CLASS_NAME		L"KiwanoAppWnd"

namespace kiwano
{
	namespace
	{
		MONITORINFOEX GetMoniterInfoEx(HWND hwnd);

		void AdjustWindow(uint32_t width, uint32_t height, DWORD style, uint32_t* win_width, uint32_t* win_height);

		void ChangeFullScreenResolution(int width, int height, WCHAR* device_name);

		void RestoreResolution(WCHAR* device_name);
	}

	WindowConfig::WindowConfig(String const& title, uint32_t width, uint32_t height, uint32_t icon, bool resizable, bool fullscreen)
		: title(title)
		, width(width)
		, height(height)
		, icon(icon)
		, resizable(resizable)
		, fullscreen(fullscreen)
	{
	}

	Window::Window()
		: handle_(nullptr)
		, width_(0)
		, height_(0)
		, device_name_(nullptr)
		, is_fullscreen_(false)
		, resizable_(false)
		, mouse_cursor_(CursorType::Arrow)
	{
	}

	Window::~Window()
	{
	}

	void Window::Init(WindowConfig const& config, WNDPROC proc)
	{
		HINSTANCE hinst		= GetModuleHandleW(nullptr);
		WNDCLASSEX wcex		= { 0 };
		wcex.cbSize			= sizeof(WNDCLASSEX);
		wcex.lpszClassName	= KGE_WND_CLASS_NAME;
		wcex.style			= CS_HREDRAW | CS_VREDRAW /* | CS_DBLCLKS */;
		wcex.lpfnWndProc	= proc;
		wcex.hIcon			= nullptr;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= sizeof(LONG_PTR);
		wcex.hInstance		= hinst;
		wcex.hbrBackground	= nullptr;
		wcex.lpszMenuName	= nullptr;
		wcex.hCursor		= ::LoadCursorW(hinst, IDC_ARROW);

		if (config.icon)
		{
			wcex.hIcon = (HICON)::LoadImageW(hinst, MAKEINTRESOURCE(config.icon), IMAGE_ICON, 0, 0,
				LR_DEFAULTCOLOR | LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
		}

		::RegisterClassExW(&wcex);

		// Get the nearest monitor to this window
		HMONITOR monitor = ::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);

		// Get the target monitor info
		MONITORINFOEX monitor_info_ex;
		memset(&monitor_info_ex, 0, sizeof(MONITORINFOEX));
		monitor_info_ex.cbSize = sizeof(MONITORINFOEX);
		::GetMonitorInfoW(monitor, &monitor_info_ex);

		// Save the device name
		int len = lstrlenW(monitor_info_ex.szDevice);
		device_name_ = new wchar_t[len + 1];
		lstrcpyW(device_name_, monitor_info_ex.szDevice);

		uint32_t width = config.width;
		uint32_t height = config.height;
		int left = -1;
		int top = -1;

		resizable_ = config.resizable;
		is_fullscreen_ = config.fullscreen;

		if (is_fullscreen_)
		{
			top = monitor_info_ex.rcMonitor.top;
			left = monitor_info_ex.rcMonitor.left;

			if (width > static_cast<uint32_t>(monitor_info_ex.rcWork.right - left))
				width = static_cast<uint32_t>(monitor_info_ex.rcWork.right - left);

			if (height > static_cast<uint32_t>(monitor_info_ex.rcWork.bottom - top))
				height = static_cast<uint32_t>(monitor_info_ex.rcWork.bottom - top);
		}
		else
		{
			uint32_t screenw = monitor_info_ex.rcWork.right - monitor_info_ex.rcWork.left;
			uint32_t screenh = monitor_info_ex.rcWork.bottom - monitor_info_ex.rcWork.top;

			uint32_t win_width, win_height;
			AdjustWindow(
				width,
				height,
				GetWindowStyle(),
				&win_width,
				&win_height
			);

			left = monitor_info_ex.rcWork.left + (screenw - win_width) / 2;
			top = monitor_info_ex.rcWork.top + (screenh - win_height) / 2;
			width = win_width;
			height = win_height;
		}

		handle_ = ::CreateWindowExW(
			is_fullscreen_ ? WS_EX_TOPMOST : 0,
			KGE_WND_CLASS_NAME,
			config.title.c_str(),
			GetWindowStyle(),
			left,
			top,
			width,
			height,
			nullptr,
			nullptr,
			hinst,
			nullptr
		);

		if (handle_ == nullptr)
		{
			::UnregisterClass(KGE_WND_CLASS_NAME, hinst);
			ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
		}
		else
		{
			RECT rc;
			GetClientRect(handle_, &rc);
			width_ = rc.right - rc.left;
			height_ = rc.bottom - rc.top;
		}
	}

	void Window::Prepare()
	{
		::ShowWindow(handle_, SW_SHOWNORMAL);
		::UpdateWindow(handle_);

		if (is_fullscreen_)
		{
			ChangeFullScreenResolution(width_, height_, device_name_);
		}
	}

	void Window::PollEvents()
	{
		static MSG msg = {};

		if (::GetMessageW(&msg, nullptr, 0, 0))
		{
			::TranslateMessage(&msg);
			::DispatchMessageW(&msg);
		}
	}

	String Window::GetTitle() const
	{
		if (handle_)
		{
			wchar_t title[256];
			::GetWindowTextW(handle_, title, 256);
			return title;
		}
		return String();
	}

	void Window::SetTitle(String const& title)
	{
		if (handle_)
			::SetWindowTextW(handle_, title.c_str());
	}

	Size Window::GetSize() const
	{
		return Size{
			static_cast<float>(width_),
			static_cast<float>(height_)
		};
	}

	float Window::GetWidth() const
	{
		return static_cast<float>(width_);
	}

	float Window::GetHeight() const
	{
		return static_cast<float>(height_);
	}

	void Window::SetIcon(uint32_t icon_resource)
	{
		if (handle_)
		{
			HINSTANCE hinstance = GetModuleHandle(nullptr);
			HICON icon = (HICON)::LoadImageW(
				hinstance,
				MAKEINTRESOURCE(icon_resource),
				IMAGE_ICON,
				0,
				0,
				LR_DEFAULTCOLOR | LR_CREATEDIBSECTION | LR_DEFAULTSIZE
			);

			::SendMessage(handle_, WM_SETICON, ICON_BIG, (LPARAM)icon);
			::SendMessage(handle_, WM_SETICON, ICON_SMALL, (LPARAM)icon);
		}
	}

	void Window::Resize(int width, int height)
	{
		if (handle_ && !is_fullscreen_)
		{
			RECT rc = { 0, 0, int(width), int(height) };
			::AdjustWindowRect(&rc, GetWindowStyle(), false);

			width = rc.right - rc.left;
			height = rc.bottom - rc.top;
			::SetWindowPos(handle_, 0, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}

	void Window::SetFullscreen(bool fullscreen, int width, int height)
	{
		if (is_fullscreen_ != fullscreen || width != width_ || height != height_)
		{
			is_fullscreen_ = fullscreen;

			if (is_fullscreen_)
			{
				// move window to (0, 0) before display switch
				::SetWindowPos(handle_, HWND_TOPMOST, 0, 0, width_, height_, SWP_NOACTIVATE);

				ChangeFullScreenResolution(width, height, device_name_);

				MONITORINFOEX info = GetMoniterInfoEx(handle_);

				::SetWindowLongPtr(handle_, GWL_STYLE, GetWindowStyle());
				::SetWindowPos(handle_, HWND_TOPMOST, info.rcMonitor.top, info.rcMonitor.left, width, height, SWP_NOACTIVATE);

				width_ = width;
				height_ = height;
			}
			else
			{
				RestoreResolution(device_name_);

				MONITORINFOEX info = GetMoniterInfoEx(handle_);

				uint32_t screenw = info.rcWork.right - info.rcWork.left;
				uint32_t screenh = info.rcWork.bottom - info.rcWork.top;

				uint32_t win_width, win_height;
				AdjustWindow(width, height, GetWindowStyle(), &win_width, &win_height);

				int left = screenw > win_width ? ((screenw - win_width) / 2) : 0;
				int top = screenh > win_height ? ((screenh - win_height) / 2) : 0;

				::SetWindowLongPtr(handle_, GWL_STYLE, GetWindowStyle());
				::SetWindowPos(handle_, HWND_NOTOPMOST, left, top, win_width, win_height, SWP_DRAWFRAME | SWP_FRAMECHANGED);
				
				UpdateWindowRect();
			}
			
			::ShowWindow(handle_, SW_SHOWNORMAL);
		}
	}

	void Window::SetCursor(CursorType cursor)
	{
		mouse_cursor_ = cursor;
	}

	HWND Window::GetHandle() const
	{
		return handle_;
	}

	DWORD Window::GetWindowStyle() const
	{
		return is_fullscreen_ ? (WINDOW_FULLSCREEN_STYLE) : (resizable_ ? (WINDOW_RESIZABLE_STYLE) : (WINDOW_FIXED_STYLE));
	}

	void Window::UpdateWindowRect()
	{
		if (!handle_)
			return;

		RECT rc;
		::GetClientRect(handle_, &rc);

		width_ = rc.right - rc.left;
		height_ = rc.bottom - rc.top;
	}

	void Window::UpdateCursor()
	{
		LPTSTR win32_cursor = IDC_ARROW;
		switch (mouse_cursor_)
		{
		case CursorType::Arrow:		win32_cursor = IDC_ARROW; break;
		case CursorType::TextInput:	win32_cursor = IDC_IBEAM; break;
		case CursorType::SizeAll:	win32_cursor = IDC_SIZEALL; break;
		case CursorType::SizeWE:	win32_cursor = IDC_SIZEWE; break;
		case CursorType::SizeNS:	win32_cursor = IDC_SIZENS; break;
		case CursorType::SizeNESW:	win32_cursor = IDC_SIZENESW; break;
		case CursorType::SizeNWSE:	win32_cursor = IDC_SIZENWSE; break;
		case CursorType::Hand:		win32_cursor = IDC_HAND; break;
		}
		::SetCursor(::LoadCursorW(nullptr, win32_cursor));
	}

	void Window::SetActive(bool actived)
	{
		if (!handle_)
			return;

		if (is_fullscreen_)
		{
			if (actived)
			{
				ChangeFullScreenResolution(width_, height_, device_name_);

				MONITORINFOEX info = GetMoniterInfoEx(handle_);
				::SetWindowPos(handle_, HWND_TOPMOST, info.rcMonitor.top, info.rcMonitor.left, width_, height_, SWP_NOACTIVATE);
			}
			else
			{
				RestoreResolution(device_name_);

				::SetWindowPos(handle_, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
				::ShowWindow(handle_, SW_MINIMIZE);
			}
		}
	}

	void Window::Destroy()
	{
		if (is_fullscreen_)
			RestoreResolution(device_name_);

		if (device_name_)
		{
			delete[] device_name_;
			device_name_ = nullptr;
		}

		if (handle_)
		{
			::DestroyWindow(handle_);
			handle_ = nullptr;
		}
	}

	namespace
	{
		MONITORINFOEX GetMoniterInfoEx(HWND hwnd)
		{
			HMONITOR monitor = ::MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
			MONITORINFOEX monitor_info;

			memset(&monitor_info, 0, sizeof(MONITORINFOEX));
			monitor_info.cbSize = sizeof(MONITORINFOEX);
			::GetMonitorInfoW(monitor, &monitor_info);

			return monitor_info;
		}

		void AdjustWindow(uint32_t width, uint32_t height, DWORD style, uint32_t* win_width, uint32_t* win_height)
		{
			RECT rc;
			::SetRect(&rc, 0, 0, (int)width, (int)height);
			::AdjustWindowRect(&rc, style, false);

			*win_width = rc.right - rc.left;
			*win_height = rc.bottom - rc.top;

			MONITORINFOEX info = GetMoniterInfoEx(NULL);

			uint32_t screenw = info.rcWork.right - info.rcWork.left;
			uint32_t screenh = info.rcWork.bottom - info.rcWork.top;

			if (*win_width > screenw)
				*win_width = screenw;
			if (*win_height > screenh)
				*win_height = screenh;
		}

		void ChangeFullScreenResolution(int width, int height, WCHAR* device_name)
		{
			DEVMODE mode;

			memset(&mode, 0, sizeof(mode));
			mode.dmSize = sizeof(DEVMODE);
			mode.dmBitsPerPel = ::GetDeviceCaps(::GetDC(0), BITSPIXEL);;
			mode.dmPelsWidth = width;
			mode.dmPelsHeight = height;
			mode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

			if (::ChangeDisplaySettingsExW(device_name, &mode, NULL, CDS_FULLSCREEN, NULL) != DISP_CHANGE_SUCCESSFUL)
				KGE_ERROR(L"ChangeDisplaySettings failed");
		}

		void RestoreResolution(WCHAR* device_name)
		{
			::ChangeDisplaySettingsExW(device_name, NULL, NULL, 0, NULL);
		}
	}

}
