/*
  Joystick.cpp

  Copyright (c) 2015-2017, Matthew Heironimus

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "JoystickS418.h"
#include "FFBDescriptor.h"
#if defined(_USING_DYNAMIC_HID)

#define JOYSTICK_REPORT_ID_INDEX 7
#define JOYSTICK_AXIS_MINIMUM -32767
#define JOYSTICK_AXIS_MAXIMUM 32767
#define JOYSTICK_SIMULATOR_MINIMUM -32767
#define JOYSTICK_SIMULATOR_MAXIMUM 32767

// Axis Flags
#define JOYSTICK_INCLUDE_X_AXIS         0x00000001
#define JOYSTICK_INCLUDE_Y_AXIS         0x00000002
#define JOYSTICK_INCLUDE_Z_AXIS         0x00000004
#define JOYSTICK_INCLUDE_RX_AXIS        0x00000008
#define JOYSTICK_INCLUDE_RY_AXIS        0x00000010
#define JOYSTICK_INCLUDE_RZ_AXIS        0x00000020
#define JOYSTICK_INCLUDE_SLIDER         0x00000040
#define JOYSTICK_INCLUDE_DIAL           0x00000080
#define JOYSTICK_INCLUDE_WHEEL          0x00000100
#define JOYSTICK_INCLUDE_HATSWITCH      0x00000200
#define JOYSTICK_INCLUDE_VX             0x00000400
#define JOYSTICK_INCLUDE_VY             0x00000800
#define JOYSTICK_INCLUDE_VZ             0x00001000
#define JOYSTICK_INCLUDE_VBRX           0x00002000
#define JOYSTICK_INCLUDE_VBRY           0x00004000
#define JOYSTICK_INCLUDE_VBRZ           0x00008000
#define JOYSTICK_INCLUDE_AX             0x00010000
#define JOYSTICK_INCLUDE_AY             0x00020000
#define JOYSTICK_INCLUDE_AZ             0x00040000
#define JOYSTICK_INCLUDE_ABRRX          0x00080000
#define JOYSTICK_INCLUDE_ABRRY          0x00100000
#define JOYSTICK_INCLUDE_ABRRZ          0x00200000
#define JOYSTICK_INCLUDE_FORCEX         0x00400000
#define JOYSTICK_INCLUDE_FORCEY         0x00800000
#define JOYSTICK_INCLUDE_FORCEZ         0x01000000
#define JOYSTICK_INCLUDE_TORQUEX        0x02000000
#define JOYSTICK_INCLUDE_TORQUEY        0x04000000
#define JOYSTICK_INCLUDE_TORQUEZ        0x08000000

// Simulator Flags
#define JOYSTICK_INCLUDE_YAW            0x00000001
#define JOYSTICK_INCLUDE_PITCH          0x00000002
#define JOYSTICK_INCLUDE_ROLL           0x00000004
#define JOYSTICK_INCLUDE_RUDDER         0x00000008
#define JOYSTICK_INCLUDE_THROTTLE       0x00000010
#define JOYSTICK_INCLUDE_ACCELERATOR    0x00000020
#define JOYSTICK_INCLUDE_BRAKE          0x00000040
#define JOYSTICK_INCLUDE_CLUTCH         0x00000080
#define JOYSTICK_INCLUDE_HANDBRAKE      0x00000100
#define JOYSTICK_INCLUDE_STEERING       0x00000200
#define JOYSTICK_INCLUDE_TURRETX        0x00000400
#define JOYSTICK_INCLUDE_TURRETY        0x00000800
#define JOYSTICK_INCLUDE_TURRETZ        0x00001000

unsigned int timecnt = 0;

namespace S418 {
    namespace JoystickFfb {

Joystick_::Joystick_()
{
	_includeAxisFlags = 0;
	_includeSimulatorFlags = 0;

this->hidReportId(JOYSTICK_DEFAULT_REPORT_ID)
        .joystickType(JOYSTICK_TYPE_JOYSTICK)
        .buttonCount(0)
        .hatSwitchCount(0)
        ;

        for(size_t i=0; i < ALL_AXES_COUNT; i++) {
            this
                ->includeAxis(ALL_AXES[i], false)
                .setAxisRange(ALL_AXES[i], 0, 1023)
                .setAxisValue(ALL_AXES[i], 0);
        }
}

Joystick_& Joystick_::init()
{
    // Build Joystick HID Report Description

    // Button Calculations
    uint8_t buttonsInLastByte = _buttonCount % 8;
    uint8_t buttonPaddingBits = 0;
    if (buttonsInLastByte > 0)
    {
        buttonPaddingBits = 8 - buttonsInLastByte;
    }

    uint8_t axisCount = __builtin_popcount(_includeAxisFlags);
    uint8_t simulationCount = __builtin_popcount(_includeSimulatorFlags);

    static uint8_t tempHidReportDescriptor[200];
    int hidReportDescriptorSize = 0;

    // USAGE_PAGE (Generic Desktop)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x05;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

    // USAGE (Joystick - 0x04; Gamepad - 0x05; Multi-axis Controller - 0x08)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
    tempHidReportDescriptor[hidReportDescriptorSize++] = _joystickType;

    // COLLECTION (Application)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0xa1;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;
	// USAGE (Pointer)
	tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
	tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;
    // REPORT_ID (Default: 1)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x85;
    tempHidReportDescriptor[hidReportDescriptorSize++] = _hidReportId;

    // COLLECTION (Physical)
	tempHidReportDescriptor[hidReportDescriptorSize++] = 0xa1;
	tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

    if (_buttonCount > 0) {
        // USAGE_PAGE (Button)
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x05;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;

        // USAGE_MINIMUM (Button 1)
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x19;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

        // USAGE_MAXIMUM (Button 32)
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x29;
        tempHidReportDescriptor[hidReportDescriptorSize++] = _buttonCount;

        // LOGICAL_MINIMUM (0)
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x15;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

        // LOGICAL_MAXIMUM (1)
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x25;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

        // REPORT_SIZE (1)
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x75;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

        // REPORT_COUNT (# of buttons)
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x95;
        tempHidReportDescriptor[hidReportDescriptorSize++] = _buttonCount;

		// UNIT_EXPONENT (0)
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x55;
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

		// UNIT (None)
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x65;
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

        // INPUT (Data,Var,Abs)
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x81;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02;

        if (buttonPaddingBits > 0) {
            // REPORT_SIZE (1)
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x75;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

            // REPORT_COUNT (# of padding bits)
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x95;
            tempHidReportDescriptor[hidReportDescriptorSize++] = buttonPaddingBits;

            // INPUT (Const,Var,Abs)
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x81;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x03;

		} // Padding Bits Needed
    } // Buttons

    if ((axisCount > 0) || (_hatSwitchCount > 0)) {
		// USAGE_PAGE (Generic Desktop)
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x05;
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;
	}

    if (_hatSwitchCount > 0) {

		// USAGE (Hat Switch)
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x39;

		// LOGICAL_MINIMUM (0)
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x15;
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

		// LOGICAL_MAXIMUM (7)
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x25;
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x07;

		// PHYSICAL_MINIMUM (0)
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x35;
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

		// PHYSICAL_MAXIMUM (315)
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x46;
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x3B;
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

		// UNIT (Eng Rot:Angular Pos)
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x65;
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x14;

		// REPORT_SIZE (4)
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x75;
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x04;

		// REPORT_COUNT (1)
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x95;
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

		// INPUT (Data,Var,Abs)
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x81;
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02;

		if (_hatSwitchCount > 1) {

			// USAGE (Hat Switch)
			tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
			tempHidReportDescriptor[hidReportDescriptorSize++] = 0x39;

			// LOGICAL_MINIMUM (0)
			tempHidReportDescriptor[hidReportDescriptorSize++] = 0x15;
			tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

			// LOGICAL_MAXIMUM (7)
			tempHidReportDescriptor[hidReportDescriptorSize++] = 0x25;
			tempHidReportDescriptor[hidReportDescriptorSize++] = 0x07;

			// PHYSICAL_MINIMUM (0)
			tempHidReportDescriptor[hidReportDescriptorSize++] = 0x35;
			tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

			// PHYSICAL_MAXIMUM (315)
			tempHidReportDescriptor[hidReportDescriptorSize++] = 0x46;
			tempHidReportDescriptor[hidReportDescriptorSize++] = 0x3B;
			tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

			// UNIT (Eng Rot:Angular Pos)
			tempHidReportDescriptor[hidReportDescriptorSize++] = 0x65;
			tempHidReportDescriptor[hidReportDescriptorSize++] = 0x14;

			// REPORT_SIZE (4)
			tempHidReportDescriptor[hidReportDescriptorSize++] = 0x75;
			tempHidReportDescriptor[hidReportDescriptorSize++] = 0x04;

			// REPORT_COUNT (1)
			tempHidReportDescriptor[hidReportDescriptorSize++] = 0x95;
			tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

			// INPUT (Data,Var,Abs)
			tempHidReportDescriptor[hidReportDescriptorSize++] = 0x81;
			tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02;

		} else {

			// Use Padding Bits

			// REPORT_SIZE (1)
			tempHidReportDescriptor[hidReportDescriptorSize++] = 0x75;
			tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

			// REPORT_COUNT (4)
			tempHidReportDescriptor[hidReportDescriptorSize++] = 0x95;
			tempHidReportDescriptor[hidReportDescriptorSize++] = 0x04;

			// INPUT (Const,Var,Abs)
			tempHidReportDescriptor[hidReportDescriptorSize++] = 0x81;
			tempHidReportDescriptor[hidReportDescriptorSize++] = 0x03;

		} // One or Two Hat Switches?

	} // Hat Switches

    if (axisCount > 0) {

		// USAGE (Pointer)
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

		// LOGICAL_MINIMUM (-32767)
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x16;
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x80;

		// LOGICAL_MAXIMUM (+32767)
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x26;
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0xFF;
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x7F;

		// REPORT_SIZE (16)
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x75;
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x10;

		// REPORT_COUNT (axisCount)
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x95;
		tempHidReportDescriptor[hidReportDescriptorSize++] = axisCount;

		// COLLECTION (Physical)
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0xA1;
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

        // Add axes USAGEs
        // USAGEs (Based on _includeAxisFlags)
        uint32_t flag = 1;
        uint8_t usageCount = 0;
        for (uint8_t i = 0; i < 32 && usageCount < axisCount; i++) {
            if (_includeAxisFlags & flag) {
                // USAGE (usage)
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
                switch (flag) {
                    case JOYSTICK_INCLUDE_X_AXIS: tempHidReportDescriptor[hidReportDescriptorSize++] = 0x30; break;
                    case JOYSTICK_INCLUDE_Y_AXIS: tempHidReportDescriptor[hidReportDescriptorSize++] = 0x31; break;
                    case JOYSTICK_INCLUDE_Z_AXIS: tempHidReportDescriptor[hidReportDescriptorSize++] = 0x32; break;
                    case JOYSTICK_INCLUDE_RX_AXIS: tempHidReportDescriptor[hidReportDescriptorSize++] = 0x33; break;
                    case JOYSTICK_INCLUDE_RY_AXIS: tempHidReportDescriptor[hidReportDescriptorSize++] = 0x34; break;
                    case JOYSTICK_INCLUDE_RZ_AXIS: tempHidReportDescriptor[hidReportDescriptorSize++] = 0x35; break;
                    case JOYSTICK_INCLUDE_SLIDER: tempHidReportDescriptor[hidReportDescriptorSize++] = 0x36; break;
                    case JOYSTICK_INCLUDE_DIAL: tempHidReportDescriptor[hidReportDescriptorSize++] = 0x37; break;
                    case JOYSTICK_INCLUDE_WHEEL: tempHidReportDescriptor[hidReportDescriptorSize++] = 0x38; break;
                    case JOYSTICK_INCLUDE_HATSWITCH:    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x39; break;
                    case JOYSTICK_INCLUDE_VX: tempHidReportDescriptor[hidReportDescriptorSize++] = 0x40; break;
                    case JOYSTICK_INCLUDE_VY: tempHidReportDescriptor[hidReportDescriptorSize++] = 0x41; break;
                    case JOYSTICK_INCLUDE_VZ: tempHidReportDescriptor[hidReportDescriptorSize++] = 0x42; break;
                    case JOYSTICK_INCLUDE_VBRX: tempHidReportDescriptor[hidReportDescriptorSize++] = 0x43; break;
                    case JOYSTICK_INCLUDE_VBRY: tempHidReportDescriptor[hidReportDescriptorSize++] = 0x44; break;
                    case JOYSTICK_INCLUDE_VBRZ: tempHidReportDescriptor[hidReportDescriptorSize++] = 0x45; break;
                    case JOYSTICK_INCLUDE_AX: tempHidReportDescriptor[hidReportDescriptorSize++] = 0x46; break;
                    case JOYSTICK_INCLUDE_AY: tempHidReportDescriptor[hidReportDescriptorSize++] = 0x47; break;
                    case JOYSTICK_INCLUDE_AZ: tempHidReportDescriptor[hidReportDescriptorSize++] = 0x48; break;
                    case JOYSTICK_INCLUDE_ABRRX: tempHidReportDescriptor[hidReportDescriptorSize++] = 0x49; break;
                    case JOYSTICK_INCLUDE_ABRRY: tempHidReportDescriptor[hidReportDescriptorSize++] = 0x4A; break;
                    case JOYSTICK_INCLUDE_ABRRZ: tempHidReportDescriptor[hidReportDescriptorSize++] = 0x4B; break;
                    case JOYSTICK_INCLUDE_FORCEX: tempHidReportDescriptor[hidReportDescriptorSize++] = 0x4C; break;
                    case JOYSTICK_INCLUDE_FORCEY: tempHidReportDescriptor[hidReportDescriptorSize++] = 0x4D; break;
                    case JOYSTICK_INCLUDE_FORCEZ: tempHidReportDescriptor[hidReportDescriptorSize++] = 0x4E; break;
                    case JOYSTICK_INCLUDE_TORQUEX: tempHidReportDescriptor[hidReportDescriptorSize++] = 0x4F; break;
                    case JOYSTICK_INCLUDE_TORQUEY: tempHidReportDescriptor[hidReportDescriptorSize++] = 0x50; break;
                    case JOYSTICK_INCLUDE_TORQUEZ: tempHidReportDescriptor[hidReportDescriptorSize++] = 0x51; break;
                    default: tempHidReportDescriptor[hidReportDescriptorSize++] = 0x30; break;
                }
                usageCount++;
            }
            flag <<= 1;
        }

        // INPUT (Data,Var,Abs)
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x81;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02;

        // END_COLLECTION (Physical)
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0xc0;
    }

    // Simulation Controls Collection
    if (simulationCount > 0) {

		// USAGE_PAGE (Simulation Controls)
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x05;
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02;

		// LOGICAL_MINIMUM (-32767)
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x16;
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x80;

		// LOGICAL_MAXIMUM (+32767)
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x26;
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0xFF;
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x7F;

		// REPORT_SIZE (16)
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x75;
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x10;

		// REPORT_COUNT (simulationCount)
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x95;
		tempHidReportDescriptor[hidReportDescriptorSize++] = simulationCount;

		// COLLECTION (Physical)
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0xA1;
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

        // Add simulation USAGEs
        // USAGEs (Based on _includeSimulatorFlags)
        uint32_t simFlag = 1;
        uint8_t simUsageCount = 0;
        for (uint8_t i = 0; i < 32 && simUsageCount < simulationCount; i++) {
            if (_includeSimulatorFlags & simFlag) {
                // USAGE (usage)
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
                switch (simFlag) {
                    case JOYSTICK_INCLUDE_YAW: tempHidReportDescriptor[hidReportDescriptorSize++] = 0xB0; break;
                    case JOYSTICK_INCLUDE_PITCH: tempHidReportDescriptor[hidReportDescriptorSize++] = 0xB1; break;
                    case JOYSTICK_INCLUDE_ROLL: tempHidReportDescriptor[hidReportDescriptorSize++] = 0xB2; break;
                    case JOYSTICK_INCLUDE_RUDDER: tempHidReportDescriptor[hidReportDescriptorSize++] = 0xBA; break;
                    case JOYSTICK_INCLUDE_THROTTLE: tempHidReportDescriptor[hidReportDescriptorSize++] = 0xBB; break;
                    case JOYSTICK_INCLUDE_ACCELERATOR: tempHidReportDescriptor[hidReportDescriptorSize++] = 0xC4; break;
                    case JOYSTICK_INCLUDE_BRAKE: tempHidReportDescriptor[hidReportDescriptorSize++] = 0xC5; break;
                    case JOYSTICK_INCLUDE_CLUTCH: tempHidReportDescriptor[hidReportDescriptorSize++] = 0xC6; break;
                    case JOYSTICK_INCLUDE_HANDBRAKE: tempHidReportDescriptor[hidReportDescriptorSize++] = 0xC7; break;
                    case JOYSTICK_INCLUDE_STEERING: tempHidReportDescriptor[hidReportDescriptorSize++] = 0xC8; break;
                    case JOYSTICK_INCLUDE_TURRETX: tempHidReportDescriptor[hidReportDescriptorSize++] = 0xC9; break;
                    case JOYSTICK_INCLUDE_TURRETY: tempHidReportDescriptor[hidReportDescriptorSize++] = 0xCA; break;
                    case JOYSTICK_INCLUDE_TURRETZ: tempHidReportDescriptor[hidReportDescriptorSize++] = 0xCB; break;
                    default: tempHidReportDescriptor[hidReportDescriptorSize++] = 0xB0; break;
                }
                simUsageCount++;
            }
            simFlag <<= 1;
        }

        // INPUT (Data,Var,Abs)
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x81;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02;

        // END_COLLECTION (Physical)
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0xc0;
    }

    // END_COLLECTION (Application)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0xc0;

    // Create a copy of the HID Report Descriptor template that is just the right size
    uint8_t *customHidReportDescriptor = new uint8_t[hidReportDescriptorSize];
    memcpy(customHidReportDescriptor, tempHidReportDescriptor, hidReportDescriptorSize);
	// Register HID Report Description
	DynamicHIDSubDescriptor* node = new DynamicHIDSubDescriptor(customHidReportDescriptor, hidReportDescriptorSize, pidReportDescriptor, pidReportDescriptorSize, false);

    DynamicHID().AppendDescriptor(node);

    // Setup Joystick State
	if (_buttonCount > 0) {
		_buttonValuesArraySize = _buttonCount / 8;
		if ((_buttonCount % 8) > 0) {
			_buttonValuesArraySize++;
		}
		_buttonValues = new uint8_t[_buttonValuesArraySize];
	}

	// Calculate HID Report Size
	_hidReportSize = _buttonValuesArraySize;
	_hidReportSize += (_hatSwitchCount > 0);
	_hidReportSize += (axisCount * 2);
	_hidReportSize += (simulationCount * 2);

    for(uint8_t i = 0; i < ALL_AXES_COUNT; i++) {
        _axes[i].value = 0;
    }

    // Initialize Hat Switch Values
    for (int index = 0; index < JOYSTICK_HATSWITCH_COUNT_MAXIMUM; index++)
    {
        _hatSwitchValues[index] = JOYSTICK_HATSWITCH_RELEASE;
    }
    for (int index = 0; index < _buttonValuesArraySize; index++)
    {
        _buttonValues[index] = 0;
    }

    return *this;
}

Joystick_& Joystick_::hidReportId(uint8_t reportId) {
    _hidReportId = reportId;
    return *this;
}

Joystick_& Joystick_::joystickType(uint8_t type) {
    _joystickType = type;
    return *this;
}

Joystick_& Joystick_::buttonCount(uint8_t count) {
    _buttonCount = count;
    return *this;
}

Joystick_& Joystick_::hatSwitchCount(uint8_t count) {
    _hatSwitchCount = count > JOYSTICK_HATSWITCH_COUNT_MAXIMUM ? JOYSTICK_HATSWITCH_COUNT_MAXIMUM : count;
    return *this;
}

Joystick_& Joystick_::setAxisRange(Axis axis, int16_t min, int16_t max)
{
    if (axis >= 0 && axis < ALL_AXES_COUNT) {
    _axes[axis].minimum = min;
    _axes[axis].maximum = max;
    }
    return *this;
}

uint32_t Joystick_::getFlagByAxis(Axis axis) {
    switch (axis) {
        case A_X:          return JOYSTICK_INCLUDE_X_AXIS;
        case A_Y:          return JOYSTICK_INCLUDE_Y_AXIS;
        case A_Z:          return JOYSTICK_INCLUDE_Z_AXIS;
        case A_RX:         return JOYSTICK_INCLUDE_RX_AXIS;
        case A_RY:         return JOYSTICK_INCLUDE_RY_AXIS;
        case A_RZ:         return JOYSTICK_INCLUDE_RZ_AXIS;
        case A_SLIDER:     return JOYSTICK_INCLUDE_SLIDER;
        case A_DIAL:       return JOYSTICK_INCLUDE_DIAL;
        case A_WHEEL:      return JOYSTICK_INCLUDE_WHEEL;
//        case A_HATSWITCH:  return JOYSTICK_INCLUDE_HATSWITCH;
//        case A_VX:         return JOYSTICK_INCLUDE_VX;
//        case A_VY:         return JOYSTICK_INCLUDE_VY;
//        case A_VZ:         return JOYSTICK_INCLUDE_VZ;
//        case A_VBRX:       return JOYSTICK_INCLUDE_VBRX;
//        case A_VBRY:       return JOYSTICK_INCLUDE_VBRY;
//        case A_VBRZ:       return JOYSTICK_INCLUDE_VBRZ;
//        case A_AX:         return JOYSTICK_INCLUDE_AX;
//        case A_AY:         return JOYSTICK_INCLUDE_AY;
//        case A_AZ:         return JOYSTICK_INCLUDE_AZ;
//        case A_ABRRX:      return JOYSTICK_INCLUDE_ABRRX;
//        case A_ABRRY:      return JOYSTICK_INCLUDE_ABRRY;
//        case A_ABRRZ:      return JOYSTICK_INCLUDE_ABRRZ;
//        case A_FORCEX:     return JOYSTICK_INCLUDE_FORCEX;
//        case A_FORCEY:     return JOYSTICK_INCLUDE_FORCEY;
//        case A_FORCEZ:     return JOYSTICK_INCLUDE_FORCEZ;
//        case A_TORQUEX:    return JOYSTICK_INCLUDE_TORQUEX;
//        case A_TORQUEY:    return JOYSTICK_INCLUDE_TORQUEY;
//        case A_TORQUEZ:    return JOYSTICK_INCLUDE_TORQUEZ;
        case S_YAW:        return JOYSTICK_INCLUDE_YAW;
        case S_PITCH:      return JOYSTICK_INCLUDE_PITCH;
        case S_ROLL:       return JOYSTICK_INCLUDE_ROLL;
        case S_RUDDER:     return JOYSTICK_INCLUDE_RUDDER;
        case S_THROTTLE:   return JOYSTICK_INCLUDE_THROTTLE;
        case S_ACCELERATOR:return JOYSTICK_INCLUDE_ACCELERATOR;
        case S_BRAKE:      return JOYSTICK_INCLUDE_BRAKE;
        case S_CLUTCH:     return JOYSTICK_INCLUDE_CLUTCH;
        case S_HANDBRAKE:  return JOYSTICK_INCLUDE_HANDBRAKE;
        case S_STEERING:   return JOYSTICK_INCLUDE_STEERING;
        case S_TURRETX:    return JOYSTICK_INCLUDE_TURRETX;
        case S_TURRETY:    return JOYSTICK_INCLUDE_TURRETY;
        case S_TURRETZ:    return JOYSTICK_INCLUDE_TURRETZ;
        default:           return 0;
    }
}

Joystick_& Joystick_::includeAxis(Axis axis, bool include)
{
    uint32_t flag = getFlagByAxis(axis);
    if (!flag) {
        return *this;
    }
    if (isSimulationAxis(axis)) {
        _includeSimulatorFlags = include ? (_includeSimulatorFlags | flag) : (_includeSimulatorFlags & ~flag);
    } else {
        _includeAxisFlags = include ? (_includeAxisFlags | flag) : (_includeAxisFlags & ~flag);
    }
    return *this;
}

bool Joystick_::isAxisIncluded(Axis axis) {
    uint32_t flag = getFlagByAxis(axis);
    if (!flag) {
        return false;
    }

    return isSimulationAxis(axis) ? _includeSimulatorFlags & flag : _includeAxisFlags & flag;
}

Joystick_& Joystick_::setAxisValue(Axis axis, int16_t value, bool initOnly = false)
{
    if (axis >= 0 && axis < ALL_AXES_COUNT) {
        _axes[axis].value = value;
    }

    if (_autoSendState) {
        this->sendState();
    }

    return *this;
}

bool Joystick_::isSimulationAxis(Axis axis) {
    return axis >= S_YAW;
}

void Joystick_::begin(bool initAutoSendState)
{
	_autoSendState = initAutoSendState;
	sendState();
}

void Joystick_::getUSBPID()
{
	DynamicHID().RecvfromUsb();
}

void Joystick_::getForce(int32_t* forces) 
{
	forceCalculator(forces);
}

int32_t Joystick_::getEffectForce(volatile TEffectState& effect, Gains _gains, EffectParams _effect_params, uint8_t axis){
	uint8_t direction;
    uint8_t condition;
	bool useForceDirectionForConditionEffect = (effect.enableAxis == DIRECTION_ENABLE && effect.conditionBlocksCount == 1);

    if (effect.enableAxis == DIRECTION_ENABLE)
    {
        direction = effect.directionX;
		if (effect.conditionBlocksCount > 1) {
            condition = axis;
        } else {
	        condition = 0; // only one Condition Parameter Block is defined
		}
    }
    else
    {
        direction = axis == 0 ? effect.directionX : effect.directionY;
        condition = axis;
    }

    float angle = (direction * 360.0 / 255.0) * DEG_TO_RAD;
    float angle_ratio = axis == 0 ? sin(angle) : -1 * cos(angle);
	int32_t force = 0;
	switch (effect.effectType)
    {
	    case USB_EFFECT_CONSTANT://1
	        force = ConstantForceCalculator(effect) * _gains.constantGain * angle_ratio;
	        break;
	    case USB_EFFECT_RAMP://2
	    	force = RampForceCalculator(effect) * _gains.rampGain * angle_ratio;
	    	break;
	    case USB_EFFECT_SQUARE://3
	    	force = SquareForceCalculator(effect) * _gains.squareGain * angle_ratio;
	    	break;
	    case USB_EFFECT_SINE://4
	    	force = SinForceCalculator(effect) * _gains.sineGain * angle_ratio;
	    	break;
	    case USB_EFFECT_TRIANGLE://5
	    	force = TriangleForceCalculator(effect) * _gains.triangleGain * angle_ratio;
	    	break;
	    case USB_EFFECT_SAWTOOTHDOWN://6
	    	force = SawtoothDownForceCalculator(effect) * _gains.sawtoothdownGain * angle_ratio;
	    	break;
	    case USB_EFFECT_SAWTOOTHUP://7
	    	force = SawtoothUpForceCalculator(effect) * _gains.sawtoothupGain * angle_ratio;
	    	break;
	    case USB_EFFECT_SPRING://8
	    	force = ConditionForceCalculator(effect, NormalizeRange(_effect_params.springPosition, _effect_params.springMaxPosition), condition) * _gains.springGain;
	    	if (useForceDirectionForConditionEffect) {
				force *= angle_ratio;
			}
			break;
	    case USB_EFFECT_DAMPER://9
	    	force = ConditionForceCalculator(effect, NormalizeRange(_effect_params.damperVelocity, _effect_params.damperMaxVelocity), condition) * _gains.damperGain;
	    	if (useForceDirectionForConditionEffect) {
				force *= angle_ratio;
			}
			break;
	    case USB_EFFECT_INERTIA://10
	    	if (_effect_params.inertiaAcceleration < 0 && _effect_params.frictionPositionChange < 0) {
	    		force = ConditionForceCalculator(effect, abs(NormalizeRange(_effect_params.inertiaAcceleration, _effect_params.inertiaMaxAcceleration)), condition) * _gains.inertiaGain;
	    	}
	    	else if (_effect_params.inertiaAcceleration < 0 && _effect_params.frictionPositionChange > 0) {
	    		force = -1 * ConditionForceCalculator(effect, abs(NormalizeRange(_effect_params.inertiaAcceleration, _effect_params.inertiaMaxAcceleration)), condition) * _gains.inertiaGain;
	    	}
			if (useForceDirectionForConditionEffect) {
				force *= angle_ratio;
			}
	    	break;
	    case USB_EFFECT_FRICTION://11
	    		force = ConditionForceCalculator(effect, NormalizeRange(_effect_params.frictionPositionChange, _effect_params.frictionMaxPositionChange), condition) * _gains.frictionGain;
	    		if (useForceDirectionForConditionEffect) {
				    force *= angle_ratio;
			    }
				break;
	    case USB_EFFECT_CUSTOM://12
	    		break;
	    }
	    effect.elapsedTime = (uint64_t)millis() - effect.startTime;
		return force;
}


void Joystick_::forceCalculator(int32_t* forces) {
	forces[0] = 0;
    forces[1] = 0;
	    for (int id = 0; id < MAX_EFFECTS; id++) {
	    	volatile TEffectState& effect = DynamicHID().pidReportHandler.g_EffectStates[id];
	    	if ((effect.state == MEFFECTSTATE_PLAYING) &&
	    		((effect.elapsedTime <= effect.duration) ||
	    		(effect.duration == USB_DURATION_INFINITE)) && !DynamicHID().pidReportHandler.devicePaused)
	    	{
				forces[0] += (int32_t)(getEffectForce(effect, m_gains[0], m_effect_params[0], 0));
				forces[1] += (int32_t)(getEffectForce(effect, m_gains[1], m_effect_params[1], 1));
	    	}
	    }
	forces[0] = (int32_t)((float)1.0 * forces[0] * m_gains[0].totalGain / 10000); // each effect gain * total effect gain = 10000
	forces[1] = (int32_t)((float)1.0 * forces[1] * m_gains[1].totalGain / 10000); // each effect gain * total effect gain = 10000
	forces[0] = map(forces[0], -10000, 10000, -250, 250);
	forces[1] = map(forces[1], -10000, 10000, -250, 250);
}

int32_t Joystick_::ConstantForceCalculator(volatile TEffectState& effect) 
{
	return ApplyEnvelope(effect, (int32_t)effect.magnitude);
}

int32_t Joystick_::RampForceCalculator(volatile TEffectState& effect) 
{
	int32_t tempforce = (int32_t)(effect.startMagnitude + effect.elapsedTime * 1.0 * (effect.endMagnitude - effect.startMagnitude) / effect.duration);
	return ApplyEnvelope(effect, tempforce);
}

int32_t Joystick_::SquareForceCalculator(volatile TEffectState& effect)
{
	int16_t offset = effect.offset * 2;
	int16_t magnitude = effect.magnitude;
	uint16_t phase = effect.phase;
	uint16_t elapsedTime = effect.elapsedTime;
	uint16_t period = effect.period;

	int32_t maxMagnitude = offset + magnitude;
	int32_t minMagnitude = offset - magnitude;
	uint32_t phasetime = (phase * period) / 255;
	uint32_t timeTemp = elapsedTime + phasetime;
	uint32_t reminder = timeTemp % period;
	int32_t tempforce;
	if (reminder > (period / 2)) tempforce = minMagnitude;
	else tempforce = maxMagnitude;
	return ApplyEnvelope(effect, tempforce);
}

int32_t Joystick_::SinForceCalculator(volatile TEffectState& effect) 
{
	int16_t offset = effect.offset * 2;
	int16_t magnitude = effect.magnitude;
	uint16_t phase = effect.phase;
	uint16_t timeTemp = effect.elapsedTime;
	uint16_t period = effect.period;
	float angle = 0.0;
	if(period != 0)
		angle = ((timeTemp * 1.0 / period) * 2 * PI + (phase / 36000.0));
	float sine = sin(angle);
	int32_t tempforce = (int32_t)(sine * magnitude);
	tempforce += offset;
	return ApplyEnvelope(effect, tempforce);
}

int32_t Joystick_::TriangleForceCalculator(volatile TEffectState& effect)
{
	int16_t offset = effect.offset * 2;
	int16_t magnitude = effect.magnitude;
	uint16_t elapsedTime = effect.elapsedTime;
	uint16_t phase = effect.phase;
	uint16_t period = effect.period;
	uint16_t periodF = effect.period;

	int16_t maxMagnitude = offset + magnitude;
	int16_t minMagnitude = offset - magnitude;
	int32_t phasetime = (phase * period) / 255;
	uint32_t timeTemp = elapsedTime + phasetime;
	int32_t reminder = timeTemp % period;
	int32_t slope = ((maxMagnitude - minMagnitude) * 2) / periodF;
	int32_t tempforce = 0;
	if (reminder > (periodF / 2)) tempforce = slope * (periodF - reminder);
	else tempforce = slope * reminder;
	tempforce += minMagnitude;
	return ApplyEnvelope(effect, tempforce);
}

int32_t Joystick_::SawtoothDownForceCalculator(volatile TEffectState& effect) 
{
	int16_t offset = effect.offset * 2;
	int16_t magnitude = effect.magnitude;
	uint16_t elapsedTime = effect.elapsedTime;
	uint16_t phase = effect.phase;
	uint16_t period = effect.period;
	uint16_t periodF = effect.period;

	int16_t maxMagnitude = offset + magnitude;
	int16_t minMagnitude = offset - magnitude;
	int32_t phasetime = (phase * period) / 255;
	uint32_t timeTemp = elapsedTime + phasetime;
	int32_t reminder = timeTemp % period;
	int32_t slope = (maxMagnitude - minMagnitude) / periodF;
	int32_t tempforce = 0;
	tempforce = slope * (period - reminder);
	tempforce += minMagnitude;
	return ApplyEnvelope(effect, tempforce);
}

int32_t Joystick_::SawtoothUpForceCalculator(volatile TEffectState& effect) 
{
	int16_t offset = effect.offset * 2;
	int16_t magnitude = effect.magnitude;
	uint16_t elapsedTime = effect.elapsedTime;
	uint16_t phase = effect.phase;
	uint16_t period = effect.period;
	uint16_t periodF = effect.period;

	int16_t maxMagnitude = offset + magnitude;
	int16_t minMagnitude = offset - magnitude;
	int32_t phasetime = (phase * period) / 255;
	uint32_t timeTemp = elapsedTime + phasetime;
	int32_t reminder = timeTemp % period;
	int32_t slope = (maxMagnitude - minMagnitude) / periodF;
	int32_t tempforce = 0;
	tempforce = slope * reminder;
	tempforce += minMagnitude;
	return ApplyEnvelope(effect, tempforce);
}

int32_t Joystick_::ConditionForceCalculator(volatile TEffectState& effect, float metric, uint8_t axis)
{
	float deadBand;
	float cpOffset;
	float positiveCoefficient;
	float negativeCoefficient;
	float positiveSaturation;
	float negativeSaturation;

    deadBand = effect.conditions[axis].deadBand;
    cpOffset = effect.conditions[axis].cpOffset;
    negativeCoefficient = effect.conditions[axis].negativeCoefficient;
    negativeSaturation = effect.conditions[axis].negativeSaturation;
    positiveSaturation = effect.conditions[axis].positiveSaturation;
    positiveCoefficient = effect.conditions[axis].positiveCoefficient;

	float  tempForce = 0;
	if (metric < (cpOffset - deadBand)) 
	{
		tempForce = (metric - (float)1.00*(cpOffset - deadBand)/10000) * negativeCoefficient;
		tempForce = (tempForce < -negativeSaturation ? -negativeSaturation : tempForce);
	}
	else if (metric > (cpOffset + deadBand)) 
	{
		tempForce = (metric - (float)1.00 * (cpOffset + deadBand) / 10000) * positiveCoefficient;
		tempForce = (tempForce > positiveSaturation ? positiveSaturation : tempForce);
	}
	else return 0;
	tempForce = -tempForce * effect.gain / 255;
	switch (effect.effectType) {
	case  USB_EFFECT_DAMPER:
		//tempForce = damperFilter.filterIn(tempForce);
		break;
	case USB_EFFECT_INERTIA:
		//tempForce = interiaFilter.filterIn(tempForce);
		break;
	case USB_EFFECT_FRICTION:
		//tempForce = frictionFilter.filterIn(tempForce);
		break;
	default:
		break;
	}
	return (int32_t)tempForce;
}

inline float Joystick_::NormalizeRange(int32_t x, int32_t maxValue) {
	return (float)x * 1.00 / maxValue;
}

inline int32_t  Joystick_::ApplyGain(int16_t value, uint8_t gain)
{
	int32_t value_32 = value;
	return ((value_32 * gain) / 255);
}

inline int32_t Joystick_::ApplyEnvelope(volatile TEffectState& effect, int32_t value)
{
	int32_t magnitude = ApplyGain(effect.magnitude, effect.gain);
	int32_t attackLevel = ApplyGain(effect.attackLevel, effect.gain);
	int32_t fadeLevel = ApplyGain(effect.fadeLevel, effect.gain);
	int32_t newValue = magnitude;
	int32_t attackTime = effect.attackTime;
	int32_t fadeTime = effect.fadeTime;
	int32_t elapsedTime = effect.elapsedTime;
	int32_t duration = effect.duration;

	if (elapsedTime < attackTime)
	{
		newValue = (magnitude - attackLevel) * elapsedTime / attackTime;
		newValue += attackLevel;
	}
	if (elapsedTime > (duration - fadeTime))
	{
		newValue = (magnitude - fadeLevel) * (duration - elapsedTime);
		newValue /= fadeTime;
		newValue += fadeLevel;
	}
	newValue = newValue * value / magnitude;
	return newValue;
}

void Joystick_::end()
{
}

void Joystick_::setButton(uint8_t button, uint8_t value)
{
	if (value == 0)
	{
		releaseButton(button);
	}
	else
	{
		pressButton(button);
	}
}
void Joystick_::pressButton(uint8_t button)
{
    if (button >= _buttonCount) return;

    int index = button / 8;
    int bit = button % 8;

	bitSet(_buttonValues[index], bit);
	if (_autoSendState) sendState();
}
void Joystick_::releaseButton(uint8_t button)
{
    if (button >= _buttonCount) return;

    int index = button / 8;
    int bit = button % 8;

    bitClear(_buttonValues[index], bit);
	if (_autoSendState) sendState();
}

void Joystick_::setHatSwitch(int8_t hatSwitchIndex, int16_t value)
{
	if (hatSwitchIndex >= _hatSwitchCount) return;
	
	_hatSwitchValues[hatSwitchIndex] = value;
	if (_autoSendState) sendState();
}

int Joystick_::buildAndSet16BitValue(bool includeValue, int16_t value, int16_t valueMinimum, int16_t valueMaximum, int16_t actualMinimum, int16_t actualMaximum, uint8_t dataLocation[]) 
{
	int16_t convertedValue;
	uint8_t highByte;
	uint8_t lowByte;
	int16_t realMinimum = min(valueMinimum, valueMaximum);
	int16_t realMaximum = max(valueMinimum, valueMaximum);

	if (includeValue == false) return 0;

	if (value < realMinimum) {
		value = realMinimum;
	}
	if (value > realMaximum) {
		value = realMaximum;
	}

	if (valueMinimum > valueMaximum) {
		// Values go from a larger number to a smaller number (e.g. 1024 to 0)
		value = realMaximum - value + realMinimum;
	}

	convertedValue = map(value, realMinimum, realMaximum, actualMinimum, actualMaximum);

	highByte = (uint8_t)(convertedValue >> 8);
	lowByte = (uint8_t)(convertedValue & 0x00FF);
	
	dataLocation[0] = lowByte;
	dataLocation[1] = highByte;
	
	return 2;
}

int Joystick_::buildAndSetAxisValue(Axis axis, uint8_t dataLocation[])
{
	return buildAndSet16BitValue(
        isAxisIncluded(axis),
        _axes[axis].value,
        _axes[axis].minimum,
        _axes[axis].maximum,
        JOYSTICK_AXIS_MINIMUM,
        JOYSTICK_AXIS_MAXIMUM,
        dataLocation
    );
}

void Joystick_::sendState()
{
	uint8_t data[_hidReportSize];
	int index = 0;
	
	// Load Button State
	for (; index < _buttonValuesArraySize; index++)
	{
		data[index] = _buttonValues[index];		
	}

	// Set Hat Switch Values
	if (_hatSwitchCount > 0) {
		
		// Calculate hat-switch values
		uint8_t convertedHatSwitch[JOYSTICK_HATSWITCH_COUNT_MAXIMUM];
		for (int hatSwitchIndex = 0; hatSwitchIndex < JOYSTICK_HATSWITCH_COUNT_MAXIMUM; hatSwitchIndex++)
		{
			if (_hatSwitchValues[hatSwitchIndex] < 0)
			{
				convertedHatSwitch[hatSwitchIndex] = 8;
			}
			else
			{
				convertedHatSwitch[hatSwitchIndex] = (_hatSwitchValues[hatSwitchIndex] % 360) / 45;
			}			
		}

		// Pack hat-switch states into a single byte
		data[index++] = (convertedHatSwitch[1] << 4) | (B00001111 & convertedHatSwitch[0]);
	
	} // Hat Switches

        for(size_t i=0; i < ALL_AXES_COUNT; i++) {
            index += buildAndSetAxisValue(ALL_AXES[i], &(data[index]));
        }

	DynamicHID().SendReport(_hidReportId, data, _hidReportSize);
}
    } // namespace JoystickFfb
} // namespace S418
#endif
