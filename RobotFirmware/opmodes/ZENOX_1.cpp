#include "NRL.h"
#include <HexaDCMotor.h>
#include <TankDrive.h>
#include <HexaServo.h>
static HexaDCMotor leftMotor{{.dirPin = MOTOR_L_DIR, .pwmPin = MOTOR_L_PWM, .flipped = true}};
static HexaDCMotor rightMotor{{.dirPin = MOTOR_R_DIR, .pwmPin = MOTOR_R_PWM, .flipped = true}};
static HexaServo gripper{{.signalPin = SERVO_1, .startAngle = 40.0f, .settleMs = 0}};
static HexaServo arm{{.signalPin = SERVO_2, .startAngle = 95.0f, .settleMs = 0}};
static HexaServo S3{{.signalPin = SERVO_3, .startAngle = 5.0f, .settleMs = 0}};
static HexaServo S4{{.signalPin = SERVO_4, .startAngle = 175.0f, .settleMs = 0}};

class ZENOX_1 : public NRLOpMode
{
public:
    void init() override
    {
        // Initialize motors and servos
        leftMotor.begin();
        rightMotor.begin();
        gripper.begin();
        arm.begin();
        S3.begin();
        S4.begin();
    }

    void loop() override
    {
        // Drive controls: X axis of left joystick controls left side,
        // X axis of right joystick controls right side.
        // Joysticks return -1.0 to 1.0, scaled to motor speed range (-255 to 255).
        leftMotor.setSpeed((int)(gamepad1.leftY() * 255.0f));
        rightMotor.setSpeed((int)(gamepad1.rightY() * 255.0f));

        // Servo controls via D-pad
        // Gripper: D-pad Left = Open (45), D-pad Right = Close (110)
        if (gamepad1.pressed(BTN_RB))
        {
            gripper.setPosition(35.0f);
        }
        if (gamepad1.pressed(BTN_LB))
        {
            gripper.setPosition(110.0f);
        }

        // Arm: D-pad Down = 35, D-pad Up = 90
        if (gamepad1.pressed(BTN_DPAD_UP))
        {
            arm.setPosition(35.0f);
        }
        if (gamepad1.pressed(BTN_DPAD_DOWN))
        {
            arm.setPosition(95.0f);
        }
        if (BTN_RT == 1.0)
        {
            S3.setPosition(5.0f);
            S4.setPosition(175.0f);
        }
        if (BTN_RT == -1.0)
        {
            S3.setPosition(175.0f);
            S4.setPosition(5.0f);
        }
    }

    void stop() override
    {
        // Stop motors and relax servos when match ends
        leftMotor.setSpeed(0);
        rightMotor.setSpeed(0);
        gripper.detach();
        arm.detach();
        S3.detach();
        S4.detach();
    }
};

REGISTER_OPMODE(ZENOX_1, "Proto_1", TELEOP);
