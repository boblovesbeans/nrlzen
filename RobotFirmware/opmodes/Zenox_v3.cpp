
#include "NRL.h"


// ARM ANGLES

static const float ARM_ORIGINAL = 95.0f;
static const float ARM_FINAL = 35.0f;


// GRIPPER ANGLES

static const float GRIPPER_ORIGINAL = 35.0f;
static const float GRIPPER_FINAL = 120.0f;

// SPECIAL MODE GRIPPER ORIGINAL
static const float GRIPPER_SPECIAL_ORIGINAL = 50.0f;


// DRIVE MOTORS

static HexaDCMotor leftMotor{{.dirPin = MOTOR_L_DIR,
                              .pwmPin = MOTOR_L_PWM,
                              .flipped = true}};

static HexaDCMotor rightMotor{{.dirPin = MOTOR_R_DIR,
                               .pwmPin = MOTOR_R_PWM,
                               .flipped = true}};

// SERVOS

static HexaServo gripper{{.signalPin = SERVO_1,
                          .startAngle = GRIPPER_ORIGINAL,
                          .settleMs = 0}};

static HexaServo arm{{.signalPin = SERVO_2,
                      .startAngle = ARM_ORIGINAL,
                      .settleMs = 0}};

static HexaServo servo3{{.signalPin = SERVO_3,
                         .startAngle = 4.0f,
                         .settleMs = 0}};

static HexaServo servo4{{.signalPin = SERVO_4,
                         .startAngle = 170.0f,
                         .settleMs = 0}};


// SETTINGS

static const float SLOW_SCALE = 0.5f;
static const float LOW_BATTERY_V = 10.5f;
static const unsigned long SERVO_DELAY = 400;

class TankTeleop : public NRLOpMode
{
    bool _armOpen = false;
    bool _waitingForArm = false;
    bool _specialMode = false;

    unsigned long _gripperMoveTime = 0;

public:
    // INITIALIZATION
    void init() override
    {
        leftMotor.begin();
        rightMotor.begin();

        gripper.begin();
        arm.begin();

        servo3.begin();
        servo4.begin();
    }
    // MAIN LOOP
    void loop() override
    {
        //SPECIAL MODE TOGGLE

        if (gamepad1.justPressed(BTN_X))
        {
            _specialMode = !_specialMode;
            _waitingForArm = false;
        }

        //TANK DRIVE

        float scale = gamepad1.pressed(BTN_RB)
                          ? SLOW_SCALE
                          : 1.0f;

        int leftSpeed =
            (int)(gamepad1.leftY() * 255.0f * scale);

        int rightSpeed =
            (int)(gamepad1.rightY() * 255.0f * scale);

        leftMotor.setSpeed(leftSpeed);
        rightMotor.setSpeed(rightSpeed);

        // SERVO CONTROL

        if (_specialMode)
        {
            // DPAD UP: ARM FINAL (35 DEGREES)
            if (gamepad1.justPressed(BTN_DPAD_UP))
            {
                arm.setPosition(ARM_FINAL);
                _armOpen = true;
            }

            // DPAD DOWN: ARM ORIGINAL (95 DEGREES)
            if (gamepad1.justPressed(BTN_DPAD_DOWN))
            {
                arm.setPosition(ARM_ORIGINAL);
                _armOpen = false;
            }

            // DPAD RIGHT: GRIPPER FINAL (120 DEGREES)
            if (gamepad1.justPressed(BTN_DPAD_RIGHT))
            {
                gripper.setPosition(GRIPPER_FINAL);
            }

            // DPAD LEFT: GRIPPER SPECIAL ORIGINAL (50 DEGREES)
            if (gamepad1.justPressed(BTN_DPAD_LEFT))
            {
                gripper.setPosition(GRIPPER_SPECIAL_ORIGINAL);
            }
        }
        else
        {
            //NORMAL LB CONTROL

            if (gamepad1.justPressed(BTN_LB) && !_waitingForArm)
            {
                _armOpen = !_armOpen;

                // GRIPPER MOVES FIRST
                gripper.setPosition(
                    _armOpen ? GRIPPER_FINAL : GRIPPER_ORIGINAL);

                _gripperMoveTime = millis();
                _waitingForArm = true;
            }

            // ARM MOVES AFTER 500 MS
            if (_waitingForArm &&
                millis() - _gripperMoveTime >= SERVO_DELAY)
            {
                arm.setPosition(
                    _armOpen ? ARM_FINAL : ARM_ORIGINAL);

                _waitingForArm = false;
            }
        }

        //RIGHT TRIGGER: SERVO 3 / SERVO 4

        if (gamepad1.pressed(BTN_RT))
        {
            servo3.setPosition(124.0f);
            servo4.setPosition(50.0f);
        }
        else
        {
            servo3.setPosition(4.0f);
            servo4.setPosition(170.0f);
        }

        //BATTERY LED

        float v = power.getBatteryVoltage();

        if (v < LOW_BATTERY_V)
            userLed.setBlink(255, 0, 0, 250);
        else
            userLed.setSolid(0, 60, 0);

        //TELEMETRY

        telemetry.addData("left", (float)leftSpeed);
        telemetry.addData("right", (float)rightSpeed);
        telemetry.addData("scale", scale);
        telemetry.addData("battery", v);
        telemetry.addData("arm", _armOpen ? "FINAL" : "ORIGINAL");
        telemetry.addData("StarBurst_Mode", _specialMode ? "ON" : "OFF");
    }
    // STOP
    void stop() override
    {
        leftMotor.setSpeed(0);
        rightMotor.setSpeed(0);

        userLed.setOff();
    }
};
// REGISTER OPMODE
REGISTER_OPMODE(TankTeleop, "Zenox_v3", TELEOP);
