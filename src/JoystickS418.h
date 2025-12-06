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
    A_HATSWITCH,
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

    // Joystick State
    int16_t	                 _xAxis;
    int16_t	                 _yAxis;
    int16_t	                 _zAxis;
    int16_t	                 _rxAxis;
    int16_t	                 _ryAxis;
    int16_t	                 _rzAxis;

    int16_t                  _slider;
    int16_t                  _dial;
    int16_t                  _wheel;
    int16_t                  _hatswitch;
    int16_t                  _vx;
    int16_t                  _vy;
    int16_t                  _vz;
    int16_t                  _vbrx;
    int16_t                  _vbry;
    int16_t                  _vbrz;
    int16_t                  _ax;
    int16_t                  _ay;
    int16_t                  _az;
    int16_t                  _abrrx;
    int16_t                  _abrry;
    int16_t                  _abrrz;
    int16_t                  _forcex;
    int16_t                  _forcey;
    int16_t                  _forcez;
    int16_t                  _torquex;
    int16_t                  _torquey;
    int16_t                  _torquez;

    int16_t                  _yaw;
    int16_t                  _pitch;
    int16_t                  _roll;
    int16_t                  _rudder;
    int16_t                  _throttle;
    int16_t					 _accelerator;
    int16_t					 _brake;
    int16_t					 _clutch;
    int16_t					 _handbrake;
    int16_t					 _steering;
    int16_t                  _turretx;
    int16_t                  _turrety;
    int16_t                  _turretz;

    int16_t	                 _hatSwitchValues[JOYSTICK_HATSWITCH_COUNT_MAXIMUM];
    uint8_t                 *_buttonValues = NULL;

    // Joystick Settings
    bool                     _autoSendState;
    uint8_t                  _buttonCount;
    uint8_t                  _buttonValuesArraySize = 0;
    uint8_t					 _hatSwitchCount;
    axis_flags_t    	     _includeAxisFlags;
    simulator_flags_t        _includeSimulatorFlags;

    // Axis Minimum and Maximum
    int16_t                  _xAxisMinimum = JOYSTICK_DEFAULT_AXIS_MINIMUM;
    int16_t                  _xAxisMaximum = JOYSTICK_DEFAULT_AXIS_MAXIMUM;
    int16_t                  _yAxisMinimum = JOYSTICK_DEFAULT_AXIS_MINIMUM;
    int16_t                  _yAxisMaximum = JOYSTICK_DEFAULT_AXIS_MAXIMUM;
    int16_t                  _zAxisMinimum = JOYSTICK_DEFAULT_AXIS_MINIMUM;
    int16_t                  _zAxisMaximum = JOYSTICK_DEFAULT_AXIS_MAXIMUM;
    int16_t                  _rxAxisMinimum = JOYSTICK_DEFAULT_AXIS_MINIMUM;
    int16_t                  _rxAxisMaximum = JOYSTICK_DEFAULT_AXIS_MAXIMUM;
    int16_t                  _ryAxisMinimum = JOYSTICK_DEFAULT_AXIS_MINIMUM;
    int16_t                  _ryAxisMaximum = JOYSTICK_DEFAULT_AXIS_MAXIMUM;
    int16_t                  _rzAxisMinimum = JOYSTICK_DEFAULT_AXIS_MINIMUM;
    int16_t                  _rzAxisMaximum = JOYSTICK_DEFAULT_AXIS_MAXIMUM;
    int16_t                  _sliderMinimum = JOYSTICK_DEFAULT_AXIS_MINIMUM;
    int16_t                  _sliderMaximum = JOYSTICK_DEFAULT_AXIS_MAXIMUM;
    int16_t                  _dialMinimum = JOYSTICK_DEFAULT_AXIS_MINIMUM;
    int16_t                  _dialMaximum = JOYSTICK_DEFAULT_AXIS_MAXIMUM;
    int16_t                  _wheelMinimum = JOYSTICK_DEFAULT_AXIS_MINIMUM;
    int16_t                  _wheelMaximum = JOYSTICK_DEFAULT_AXIS_MAXIMUM;
    int16_t                  _hatswitchMinimum = JOYSTICK_DEFAULT_AXIS_MINIMUM;
    int16_t                  _hatswitchMaximum = JOYSTICK_DEFAULT_AXIS_MAXIMUM;
    int16_t                  _vxMinimum = JOYSTICK_DEFAULT_AXIS_MINIMUM;
    int16_t                  _vxMaximum = JOYSTICK_DEFAULT_AXIS_MAXIMUM;
    int16_t                  _vyMinimum = JOYSTICK_DEFAULT_AXIS_MINIMUM;
    int16_t                  _vyMaximum = JOYSTICK_DEFAULT_AXIS_MAXIMUM;
    int16_t                  _vzMinimum = JOYSTICK_DEFAULT_AXIS_MINIMUM;
    int16_t                  _vzMaximum = JOYSTICK_DEFAULT_AXIS_MAXIMUM;
    int16_t                  _vbrxMinimum = JOYSTICK_DEFAULT_AXIS_MINIMUM;
    int16_t                  _vbrxMaximum = JOYSTICK_DEFAULT_AXIS_MAXIMUM;
    int16_t                  _vbryMinimum = JOYSTICK_DEFAULT_AXIS_MINIMUM;
    int16_t                  _vbryMaximum = JOYSTICK_DEFAULT_AXIS_MAXIMUM;
    int16_t                  _vbrzMinimum = JOYSTICK_DEFAULT_AXIS_MINIMUM;
    int16_t                  _vbrzMaximum = JOYSTICK_DEFAULT_AXIS_MAXIMUM;
    int16_t                  _axMinimum = JOYSTICK_DEFAULT_AXIS_MINIMUM;
    int16_t                  _axMaximum = JOYSTICK_DEFAULT_AXIS_MAXIMUM;
    int16_t                  _ayMinimum = JOYSTICK_DEFAULT_AXIS_MINIMUM;
    int16_t                  _ayMaximum = JOYSTICK_DEFAULT_AXIS_MAXIMUM;
    int16_t                  _azMinimum = JOYSTICK_DEFAULT_AXIS_MINIMUM;
    int16_t                  _azMaximum = JOYSTICK_DEFAULT_AXIS_MAXIMUM;
    int16_t                  _abrrxMinimum = JOYSTICK_DEFAULT_AXIS_MINIMUM;
    int16_t                  _abrrxMaximum = JOYSTICK_DEFAULT_AXIS_MAXIMUM;
    int16_t                  _abrryMinimum = JOYSTICK_DEFAULT_AXIS_MINIMUM;
    int16_t                  _abrryMaximum = JOYSTICK_DEFAULT_AXIS_MAXIMUM;
    int16_t                  _abrrzMinimum = JOYSTICK_DEFAULT_AXIS_MINIMUM;
    int16_t                  _abrrzMaximum = JOYSTICK_DEFAULT_AXIS_MAXIMUM;
    int16_t                  _forcexMinimum = JOYSTICK_DEFAULT_AXIS_MINIMUM;
    int16_t                  _forcexMaximum = JOYSTICK_DEFAULT_AXIS_MAXIMUM;
    int16_t                  _forceyMinimum = JOYSTICK_DEFAULT_AXIS_MINIMUM;
    int16_t                  _forceyMaximum = JOYSTICK_DEFAULT_AXIS_MAXIMUM;
    int16_t                  _forcezMinimum = JOYSTICK_DEFAULT_AXIS_MINIMUM;
    int16_t                  _forcezMaximum = JOYSTICK_DEFAULT_AXIS_MAXIMUM;
    int16_t                  _torquexMinimum = JOYSTICK_DEFAULT_AXIS_MINIMUM;
    int16_t                  _torquexMaximum = JOYSTICK_DEFAULT_AXIS_MAXIMUM;
    int16_t                  _torqueyMinimum = JOYSTICK_DEFAULT_AXIS_MINIMUM;
    int16_t                  _torqueyMaximum = JOYSTICK_DEFAULT_AXIS_MAXIMUM;
    int16_t                  _torquezMinimum = JOYSTICK_DEFAULT_AXIS_MINIMUM;
    int16_t                  _torquezMaximum = JOYSTICK_DEFAULT_AXIS_MAXIMUM;

    // Simulator Minimum and Maximum
    int16_t                  _yawMinimum = JOYSTICK_DEFAULT_SIMULATOR_MINIMUM;
    int16_t                  _yawMaximum = JOYSTICK_DEFAULT_SIMULATOR_MAXIMUM;
    int16_t                  _pitchMinimum = JOYSTICK_DEFAULT_SIMULATOR_MINIMUM;
    int16_t                  _pitchMaximum = JOYSTICK_DEFAULT_SIMULATOR_MAXIMUM;
    int16_t                  _rollMinimum = JOYSTICK_DEFAULT_SIMULATOR_MINIMUM;
    int16_t                  _rollMaximum = JOYSTICK_DEFAULT_SIMULATOR_MAXIMUM;
    int16_t                  _rudderMinimum = JOYSTICK_DEFAULT_SIMULATOR_MINIMUM;
    int16_t                  _rudderMaximum = JOYSTICK_DEFAULT_SIMULATOR_MAXIMUM;
    int16_t                  _throttleMinimum = JOYSTICK_DEFAULT_SIMULATOR_MINIMUM;
    int16_t                  _throttleMaximum = JOYSTICK_DEFAULT_SIMULATOR_MAXIMUM;
    int16_t                  _acceleratorMinimum = JOYSTICK_DEFAULT_SIMULATOR_MINIMUM;
    int16_t                  _acceleratorMaximum = JOYSTICK_DEFAULT_SIMULATOR_MAXIMUM;
    int16_t                  _brakeMinimum = JOYSTICK_DEFAULT_SIMULATOR_MINIMUM;
    int16_t                  _brakeMaximum = JOYSTICK_DEFAULT_SIMULATOR_MAXIMUM;
    int16_t                  _clutchMinimum = JOYSTICK_DEFAULT_SIMULATOR_MINIMUM;
    int16_t                  _clutchMaximum = JOYSTICK_DEFAULT_SIMULATOR_MAXIMUM;
    int16_t                  _handbrakeMinimum = JOYSTICK_DEFAULT_SIMULATOR_MINIMUM;
    int16_t                  _handbrakeMaximum = JOYSTICK_DEFAULT_SIMULATOR_MAXIMUM;
    int16_t                  _steeringMinimum = JOYSTICK_DEFAULT_SIMULATOR_MINIMUM;
    int16_t                  _steeringMaximum = JOYSTICK_DEFAULT_SIMULATOR_MAXIMUM;
    int16_t                  _turretxMinimum = JOYSTICK_DEFAULT_SIMULATOR_MINIMUM;
    int16_t                  _turretxMaximum = JOYSTICK_DEFAULT_SIMULATOR_MAXIMUM;
    int16_t                  _turretyMinimum = JOYSTICK_DEFAULT_SIMULATOR_MINIMUM;
    int16_t                  _turretyMaximum = JOYSTICK_DEFAULT_SIMULATOR_MAXIMUM;
    int16_t                  _turretzMinimum = JOYSTICK_DEFAULT_SIMULATOR_MINIMUM;
    int16_t                  _turretzMaximum = JOYSTICK_DEFAULT_SIMULATOR_MAXIMUM;

    uint8_t                  _hidReportId;
    uint8_t                  _hidReportSize;
    uint8_t                  _joystickType;

    //force feedback gain
    Gains* m_gains;

    //force feedback effect params
    EffectParams* m_effect_params;

    //lock data
    bool is_calculating_force = true;
    constexpr static size_t ALL_AXES_COUNT = 41;
    const Axis ALL_AXES[ALL_AXES_COUNT] = {
        A_X, A_Y, A_Z, A_RX, A_RY, A_RZ,
        A_SLIDER, A_DIAL, A_WHEEL, A_HATSWITCH,
        A_VX, A_VY, A_VZ, A_VBRX, A_VBRY, A_VBRZ,
        A_AX, A_AY, A_AZ, A_ABRRX, A_ABRRY, A_ABRRZ,
        A_FORCEX, A_FORCEY, A_FORCEZ,
        A_TORQUEX, A_TORQUEY, A_TORQUEZ,
        S_YAW, S_PITCH, S_ROLL, S_RUDDER, S_THROTTLE,
        S_ACCELERATOR, S_BRAKE, S_CLUTCH, S_HANDBRAKE,
        S_STEERING, S_TURRETX, S_TURRETY, S_TURRETZ
    };

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
    int buildAndSetAxisValue(bool includeAxis, int16_t axisValue, int16_t axisMinimum, int16_t axisMaximum, uint8_t dataLocation[]);
    int buildAndSetSimulationValue(bool includeValue, int16_t value, int16_t valueMinimum, int16_t valueMaximum, uint8_t dataLocation[]);

public:
    Joystick_();

    void begin(bool initAutoSendState = true);
    void end();

    Joystick_& init();

    Joystick_& hidReportId(uint8_t reportId);
    Joystick_& joystickType(uint8_t type);
    Joystick_& buttonCount(uint8_t count);
    Joystick_& hatSwitchCount(uint8_t count);

    Joystick_& Joystick_::includeAxis(Axis axis, bool include = true);
    // todo
    // bool Joystick_::isAxisIncluded(Axis axis);
    Joystick_& Joystick_::setAxisRange(Axis axis, int16_t min, int16_t max);
    Joystick_& Joystick_::setAxisValue(Axis axis, int16_t value, bool initOnly = false);

    // Position Set Functions
    void setXAxis(int16_t value);
    void setYAxis(int16_t value);
    void setZAxis(int16_t value);
    void setRxAxis(int16_t value);
    void setRyAxis(int16_t value);
    void setRzAxis(int16_t value);
    void setSlider(int16_t value);
    void setDial(int16_t value);
    void setWheel(int16_t value);
    void setVx(int16_t value);
    void setVy(int16_t value);
    void setVz(int16_t value);
    void setVbrx(int16_t value);
    void setVbry(int16_t value);
    void setVbrz(int16_t value);
    void setAx(int16_t value);
    void setAy(int16_t value);
    void setAz(int16_t value);
    void setAbrrx(int16_t value);
    void setAbrry(int16_t value);
    void setAbrrz(int16_t value);
    void setForcex(int16_t value);
    void setForcey(int16_t value);
    void setForcez(int16_t value);
    void setTorquex(int16_t value);
    void setTorquey(int16_t value);
    void setTorquez(int16_t value);
    void setYaw(int16_t value);
    void setPitch(int16_t value);
    void setRoll(int16_t value);
    void setRudder(int16_t value);
    void setThrottle(int16_t value);
    void setAccelerator(int16_t value);
    void setBrake(int16_t value);
    void setClutch(int16_t value);
    void setHandbrake(int16_t value);
    void setSteering(int16_t value);
    void setTurretx(int16_t value);
    void setTurrety(int16_t value);
    void setTurretz(int16_t value);

    void setButton(uint8_t button, uint8_t value);
    void pressButton(uint8_t button);
    void releaseButton(uint8_t button);
    void setHatSwitch(int8_t hatSwitch, int16_t value);

    void sendState();
    // get USB PID data
    void getUSBPID();
    void getForce(int32_t* forces);
    int8_t setGains(Gains* _gains){
        if(_gains != nullptr){
            //it should be added some limition here,but im so tired,it's 2:24 A.M now!
            m_gains = _gains;
            return 0;
        }
        return -1;
    };
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
