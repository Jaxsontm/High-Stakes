#include "unused/main.h"
#include "auton.h"
#include "config.hpp"
#include "lemlib/chassis/chassis.hpp"
#include "pros/abstract_motor.hpp"
#include "pros/adi.hpp"
#include "pros/llemu.hpp"
#include "pros/misc.h"
#include "pros/motors.h"
#include "pros/motors.hpp"
#include "pros/rtos.hpp"
#include <string>
using namespace pros;


/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */


void on_center_button() {
  static bool pressed = false;
  pressed = !pressed;
  if (pressed) {
    lcd::set_text(2, "I was pressed!");
  } else {
    lcd::clear_line(2);
  }
}

void screen() {
    while (true) {
        lcd::print(0, "X: %f", chassis.getPose().x);
        lcd::print(1, "Y: %f", chassis.getPose().y);
        lcd::print(2, "Theta: %f", chassis.getPose().theta);
        delay(50);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	//lcd::initialize(); // initialize brain screen
    chassis.calibrate(); // calibrate sensors
    console.println("Initializing robot...");
    Task state_machine_task_intake(state_machine_intake);
    Task state_machine_task_mogo(state_machine_mogo);
    //Task state_machine_task_lift(state_machine_lift);
    Intake.set_brake_mode(MotorBrake::brake);
    Lift.set_encoder_units(E_MOTOR_ENCODER_DEGREES);
    Lift.set_zero_position(0);
    Task screenTask([&]() {
        lemlib::Pose pose(0, 0, 0);
        while (true) {
            // print robot location to the brain screen
            lcd::print(0, "X: %f", chassis.getPose().x); // x
            lcd::print(1, "Y: %f", chassis.getPose().y); // y
            lcd::print(2, "Theta: %f", chassis.getPose().theta);//heading
            
            // delay to save resources
            delay(50); 
        }
    });
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {
  Lift.set_brake_mode(MotorBrake::coast);
        delay(50); 
    console.println("Running auton..."); //makes the auton selector properly function
	selector.run_auton();
}
/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */

 //these are booleans that allow us to have one button toggles for pistons
 bool yPressed = false;
 bool yState = false;

 bool rightPressed = false;
 bool rightState = false;

void opcontrol() {
  //sets the brake modes for the Intake and lift
    Intake.set_brake_mode(MotorBrake::coast);
    Lift.set_brake_mode(MotorBrake::hold); 

	while (true) {
    /////////////////////////////////////////////////////////////////
    //Intake buttons

		if (controller.get_digital(E_CONTROLLER_DIGITAL_R1)) {
            request_new_state_intake(StateIntake::SCORE);
        } else if (controller.get_digital(E_CONTROLLER_DIGITAL_R2)) {
            request_new_state_intake(StateIntake::UNLOAD);
        } else if (controller.get_digital_new_press(E_CONTROLLER_DIGITAL_B)) {
            request_new_state_intake(MECH);
        } else {
            request_new_state_intake(StateIntake::BRAKE);
        }

    /////////////////////////////////////////////////////////////////
    //MoGo Mech toggle 

        if (controller.get_digital(E_CONTROLLER_DIGITAL_Y) && !yPressed && !yState) {
            request_new_state_mogo(StateMogo::dcGRAB);
            yPressed = true;
            yState = true;
        } else if (controller.get_digital(E_CONTROLLER_DIGITAL_Y) && !yPressed && yState) {
            request_new_state_mogo(StateMogo::RELEASE);
            yPressed = true;
            yState = false;
        } else if (!controller.get_digital(E_CONTROLLER_DIGITAL_Y)) {
            yPressed = false;
        }

    /////////////////////////////////////////////////////////////////
    //Intake Piston toggle    

        if (controller.get_digital(E_CONTROLLER_DIGITAL_RIGHT) && !rightPressed && !rightState) {
            intakePiston.set_value(true);
            rightPressed = true;
            rightState = true;
        } else if (controller.get_digital(E_CONTROLLER_DIGITAL_RIGHT) && !rightPressed && rightState) {
            intakePiston.set_value(false);
            rightPressed = true;
            rightState = false;
        } else if (!controller.get_digital(E_CONTROLLER_DIGITAL_RIGHT)) {
            rightPressed = false;
        }

    /////////////////////////////////////////////////////////////////
    //Lift buttons
        if (controller.get_digital(E_CONTROLLER_DIGITAL_L1)) {
            Lift.move(-127);
        } else if (controller.get_digital(E_CONTROLLER_DIGITAL_L2)){
            Lift.move(127);
        } else {
            Lift.brake();
        }
    /////////////////////////////////////////////////////////////////
    //Drivetrain Mode
    
        // get left y and right x positions
        int leftY = controller.get_analog(E_CONTROLLER_ANALOG_LEFT_Y);
        int rightY = controller.get_analog(E_CONTROLLER_ANALOG_RIGHT_Y);

        // move the robot
        chassis.tank(leftY, rightY);

        // delay to save resources
        delay(25);
     }
	}