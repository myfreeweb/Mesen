#pragma once
#include "libretro.h"
#include "../Core/IKeyManager.h"
#include "../Core/KeyManager.h"
#include "../Core/FdsSystemActionManager.h"
#include "../Core/VsSystemActionManager.h"

class LibretroKeyManager : public IKeyManager
{
private:
	retro_input_state_t _getInputState;
	retro_input_poll_t _pollInput;
	bool _mouseButtons[3] = { false, false, false };

public:
	LibretroKeyManager(retro_input_state_t getInputState, retro_input_poll_t pollInput)
	{
		_getInputState = getInputState;
		_pollInput = pollInput;
		KeyManager::RegisterKeyManager(this);
	}

	~LibretroKeyManager()
	{
		KeyManager::RegisterKeyManager(nullptr);
	}

	// Inherited via IKeyManager
	virtual void RefreshState() override
	{
		if(_pollInput) {
			_pollInput();
		}

		int32_t x = _getInputState(0, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_X);
		int32_t y = _getInputState(0, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_Y);

		x += 0x8000;
		y += 0x8000;

		KeyManager::SetMousePosition((double)x / 0x10000, (double)y / 0x10000);

		int16_t dx = _getInputState(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_X);
		int16_t dy = _getInputState(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_Y);
		KeyManager::SetMouseMovement(dx, dy);
		
		_mouseButtons[(int)MouseButton::LeftButton] = _getInputState(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_LEFT) != 0;
		_mouseButtons[(int)MouseButton::RightButton] = _getInputState(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_RIGHT) != 0;
		_mouseButtons[(int)MouseButton::MiddleButton] = _getInputState(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_MIDDLE) != 0;

		shared_ptr<FdsSystemActionManager> fdsSam = Console::GetInstance()->GetSystemActionManager<FdsSystemActionManager>();
		if(fdsSam) {
			if(_getInputState(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L)) {
				fdsSam->InsertNextDisk();
			} else if(_getInputState(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R)) {
				fdsSam->SwitchDiskSide();
			}
		}
		
		shared_ptr<VsSystemActionManager> vsSam = Console::GetInstance()->GetSystemActionManager<VsSystemActionManager>();
		if(vsSam) {
			if(_getInputState(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2)) {
				vsSam->InsertCoin(0);
			} else if(_getInputState(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2)) {
				vsSam->InsertCoin(1);
			}
		}
	}

	virtual bool IsKeyPressed(uint32_t keyCode) override
	{
		if(keyCode > 0) {
			return _getInputState(keyCode >> 8, RETRO_DEVICE_JOYPAD, 0, (keyCode - 1) & 0xFF) != 0;
		} else {
			return false;
		}
	}

	virtual void UpdateDevices() override
	{
	}

	virtual bool IsMouseButtonPressed(MouseButton button) override
	{
		return _mouseButtons[(int)button];
	}

	virtual vector<uint32_t> GetPressedKeys() override
	{
		return vector<uint32_t>();
	}
	
	virtual string GetKeyName(uint32_t keyCode) override
	{
		return string();
	}

	virtual uint32_t GetKeyCode(string keyName) override
	{
		return 0;
	}

	virtual void SetKeyState(uint16_t scanCode, bool state) override
	{
	}

	virtual void ResetKeyState() override
	{
	}

	virtual void SetDisabled(bool disabled) override
	{
	}
};