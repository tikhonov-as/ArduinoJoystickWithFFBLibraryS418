/*
  Joystick.h

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

#ifndef JOYSTICK_h
#define JOYSTICK_h

#include <DynamicHID/DynamicHID.h>

#if ARDUINO < 10606
#error The Joystick library requires Arduino IDE 1.6.6 or greater. Please update your IDE.
#endif // ARDUINO < 10606

#if ARDUINO > 10606
#if !defined(USBCON)
#error The Joystick library can only be used with a USB MCU (e.g. Arduino Leonardo, Arduino Micro, etc.).
#endif // !defined(USBCON)
#endif // ARDUINO > 10606

#if !defined(_USING_DYNAMIC_HID)

#warning "Using legacy HID core (non pluggable)"

#else // !defined(_USING_DYNAMIC_HID)

//================================================================================
//  Joystick (Gamepad)

#define JOYSTICK_DEFAULT_REPORT_ID         0x01
#define JOYSTICK_DEFAULT_BUTTON_COUNT        32
#define JOYSTICK_DEFAULT_AXIS_MINIMUM         0
#define JOYSTICK_DEFAULT_AXIS_MAXIMUM      1023
#define JOYSTICK_DEFAULT_SIMULATOR_MINIMUM    0
#define JOYSTICK_DEFAULT_SIMULATOR_MAXIMUM 1023
#define JOYSTICK_DEFAULT_HATSWITCH_COUNT      2
#define JOYSTICK_HATSWITCH_COUNT_MAXIMUM      2
#define JOYSTICK_HATSWITCH_RELEASE           -1
#define JOYSTICK_TYPE_JOYSTICK             0x04
#define JOYSTICK_TYPE_GAMEPAD              0x05
#define JOYSTICK_TYPE_MULTI_AXIS           0x08

#define DIRECTION_ENABLE                   0x04
#define X_AXIS_ENABLE                      0x01
#define Y_AXIS_ENABLE                      0x02
#define FORCE_FEEDBACK_MAXGAIN              100
#define DEG_TO_RAD              ((float)((float)3.14159265359 / 180.0))

typedef uint32_t axis_flags_t;
typedef uint32_t simulator_flags_t;

enum Axis {
    A_X,
    A_Y,
    A_Z,
    A_RX,
    A_RY,
    A_RZ,
    A_SLIDER,
    A_DIAL,
    A_WHEEL,
    A_VX,
    A_VY,
    A_VZ,
    A_VBRX,
    A_VBRY,
    A_VBRZ,
    A_AX,
    A_AY,
    A_AZ,
    A_ABRRX,
    A_ABRRY,
    A_ABRRZ,
    A_FORCEX,
    A_FORCEY,
    A_FORCEZ,
    A_TORQUEX,
    A_TORQUEY,
    A_TORQUEZ,

    S_YAW,
    S_PITCH,
    S_ROLL,
    S_RUDDER,
    S_THROTTLE,
    S_ACCELERATOR,
    S_BRAKE,
    S_CLUTCH,
    S_HANDBRAKE,
    S_STEERING,
    S_TURRETX,
    S_TURRETY,
    S_TURRETZ
};

struct Gains{
    uint8_t totalGain         = FORCE_FEEDBACK_MAXGAIN;
    uint8_t constantGain      = FORCE_FEEDBACK_MAXGAIN;
    uint8_t rampGain          = FORCE_FEEDBACK_MAXGAIN;
    uint8_t squareGain        = FORCE_FEEDBACK_MAXGAIN;
    uint8_t sineGain          = FORCE_FEEDBACK_MAXGAIN;
    uint8_t triangleGain      = FORCE_FEEDBACK_MAXGAIN;
    uint8_t sawtoothdownGain  = FORCE_FEEDBACK_MAXGAIN;
    uint8_t sawtoothupGain    = FORCE_FEEDBACK_MAXGAIN;
    uint8_t springGain        = FORCE_FEEDBACK_MAXGAIN;
    uint8_t damperGain        = FORCE_FEEDBACK_MAXGAIN;
    uint8_t inertiaGain       = FORCE_FEEDBACK_MAXGAIN;
    uint8_t frictionGain      = FORCE_FEEDBACK_MAXGAIN;
    uint8_t customGain        = FORCE_FEEDBACK_MAXGAIN;
};

struct EffectParams{
    int32_t springMaxPosition = 0;
    int32_t springPosition = 0;

    int32_t damperMaxVelocity = 0;
    int32_t damperVelocity = 0;

    int32_t inertiaMaxAcceleration = 0;
    int32_t inertiaAcceleration = 0;

    int32_t frictionMaxPositionChange = 0;
    int32_t frictionPositionChange = 0;
};


namespace S418 {
    namespace JoystickFfb {
        class Joystick_ {
private:

    const Axis A_AXES[] = {
        A_X, A_Y, A_Z, A_RX, A_RY, A_RZ,
        A_SLIDER, A_DIAL, A_WHEEL,
        A_VX, A_VY, A_VZ, A_VBRX, A_VBRY, A_VBRZ,
        A_AX, A_AY, A_AZ, A_ABRRX, A_ABRRY, A_ABRRZ,
        A_FORCEX, A_FORCEY, A_FORCEZ,
        A_TORQUEX, A_TORQUEY, A_TORQUEZ
    };
    const Axis S_AXES[] = {
        S_YAW, S_PITCH, S_ROLL, S_RUDDER, S_THROTTLE,
        S_ACCELERATOR, S_BRAKE, S_CLUTCH, S_HANDBRAKE,
        S_STEERING, S_TURRETX, S_TURRETY, S_TURRETZ
    };

    constexpr static size_t A_COUNT = sizeof(A_AXES) / sizeof(A_AXES[0]);
    constexpr static size_t SC_COUNT = sizeof(S_AXES) / sizeof(S_AXES[0]);

    int16_t    _aValues[A_COUNT];
	int16_t    _axesMin[AXES_COUNT];
	int16_t    _axesMax[AXES_COUNT];
	bool       _axesInclude[AXES_COUNT];

	int16_t	   _hatSwitchValues[JOYSTICK_HATSWITCH_COUNT_MAXIMUM];

    // Joystick State

    uint8_t                 *_buttonValues = NULL;

    // Joystick Settings
    bool                     _autoSendState;
    uint8_t                  _buttonCount;
    uint8_t                  _buttonValuesArraySize = 0;
    uint8_t					 _hatSwitchCount;
//    axis_flags_t    	     _includeAxisFlags;
//    simulator_flags_t        _includeSimulatorFlags;

    uint8_t                  _hidReportId;
    uint8_t                  _hidReportSize;
    uint8_t                  _joystickType;

    //force feedback gain
    Gains* m_gains;

    //force feedback effect params
    EffectParams* m_effect_params;

    //lock data
    bool is_calculating_force = true;

    ///force calculate funtion
    float NormalizeRange(int32_t x, int32_t maxValue);
    int32_t ApplyEnvelope(volatile TEffectState& effect, int32_t value);
    int32_t ApplyGain(int16_t value, uint8_t gain);
    int32_t ConstantForceCalculator(volatile TEffectState& effect);
    int32_t RampForceCalculator(volatile TEffectState& effect);
    int32_t SquareForceCalculator(volatile TEffectState& effect);
    int32_t SinForceCalculator(volatile TEffectState& effect);
    int32_t TriangleForceCalculator(volatile TEffectState& effect);
    int32_t SawtoothDownForceCalculator(volatile TEffectState& effect);
    int32_t SawtoothUpForceCalculator(volatile TEffectState& effect);
    int32_t ConditionForceCalculator(volatile TEffectState& effect, float metric, uint8_t axis);
    void forceCalculator(int32_t* forces);
    int32_t getEffectForce(volatile TEffectState& effect, Gains _gains, EffectParams _effect_params, uint8_t axis);
protected:
    int buildAndSet16BitValue(bool includeValue, int16_t value, int16_t valueMinimum, int16_t valueMaximum, int16_t actualMinimum, int16_t actualMaximum, uint8_t dataLocation[]);
    int buildAndSetAxisValue(Axis axis, uint8_t dataLocation[]);

public:
    Joystick_();

    void begin(bool initAutoSendState = true);
    void end();

    Joystick_& init();

    // Fluent setters
    Joystick_& hidReportId(uint8_t reportId);
    Joystick_& joystickType(uint8_t type);
    Joystick_& buttonCount(uint8_t count);
    Joystick_& hatSwitchCount(uint8_t count);

    Joystick_& includeAxis(Axis axis, bool include = true);

    inline Joystick_& setAxisRange(Axis axis, int16_t min, int16_t max) {
		if(min > max) {
			int16_t t = max;
			max = min;
			min = t;
		}
		_axesMin[axis] = min;
		_axesMax[axis] = max;

        return *this;
	}

    Joystick_& setAxis(Axis axis, int16_t value);

    void setButton(uint8_t button, uint8_t value);
    void pressButton(uint8_t button);
    void releaseButton(uint8_t button);
    void setHatSwitch(int8_t hatSwitch, int16_t value);

    void sendState();
    // get USB PID data
    void getUSBPID();
    //force feedback Interfaces
    void getForce(int32_t* forces);
    //set gain functions
    int8_t setGains(Gains* _gains){
        if(_gains != nullptr){
            //it should be added some limition here,but im so tired,it's 2:24 A.M now!
            m_gains = _gains;
            return 0;
        }
        return -1;
    };
    //set effect params funtions
    int8_t setEffectParams(EffectParams* _effect_params){
        if(_effect_params != nullptr){
            m_effect_params = _effect_params;
            return 0;
        }
        return -1;
    };
};
    } // namespace JoystickFfb
} // namespace S418

#endif // !defined(_USING_DYNAMIC_HID)
#endif // JOYSTICK_h
