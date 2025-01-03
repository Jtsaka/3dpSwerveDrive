import xinput
import time
import serial
import driveAndSteer

ser = serial.Serial('COM4', 115200, timeout=1)  #Adjust COM port and baud rate as necessary
deadzone = 3000

def apply_deadzone(value):
    if abs(value) < deadzone:
        return 0
    return value

def get_thumb_values(controller_state): #setting up joysticks
    left_thumb_x = apply_deadzone(controller_state.Gamepad.sThumbLX)
    left_thumb_y = apply_deadzone(controller_state.Gamepad.sThumbLY)
    right_thumb_x = apply_deadzone(controller_state.Gamepad.sThumbRX) 
    right_thumb_y = apply_deadzone(controller_state.Gamepad.sThumbRY)

    return left_thumb_x, left_thumb_y, right_thumb_x, right_thumb_y

def get_trigger_values(controller_state): #setting up L and R triggers
    trigger_R = controller_state.Gamepad.bLeftTrigger
    trigger_L = controller_state.Gamepad.bRightTrigger

    return trigger_R, trigger_L

def main():
    controller_id = 0  #assume controller 0

    while True:
        try:
            if ser.in_waiting > 0:
                arduino_data = ser.readline().decode('utf-8').rstrip()  #read data from Arduino, decoding from unicode, stripping to remove any unnecessary characters
                print(f"Received input from Arduino: {arduino_data}")
            
            controller_state = xinput.get_state(controller_id)
            buttons = controller_state.Gamepad.wButtons
            left_thumb_x, left_thumb_y, right_thumb_x, right_thumb_y = get_thumb_values(controller_state)
            trigger_R, trigger_L = get_trigger_values(controller_state)

            if buttons & xinput.BUTTON_A:
                ser.write("BUTTON_A\n".encode())
            if buttons & xinput.BUTTON_B:
                ser.write("BUTTON_B\n".encode())
            if buttons & xinput.BUTTON_Y:
                ser.write("BUTTON_Y\n".encode())
            if buttons & xinput.BUTTON_X:
                ser.write("BUTTON_X\n".encode())
            if left_thumb_x:
                ser.write(f"stickLX:{driveAndSteer.joystickDrive(left_thumb_x)}\n".encode())
            if left_thumb_y:
                ser.write(f"stickLY:{driveAndSteer.joystickDrive(left_thumb_y)}\n".encode())
            if right_thumb_x:
                ser.write(f"stickRX:{driveAndSteer.joystickSteer(right_thumb_x)}\n".encode())
            if right_thumb_y:
                ser.write(f"stickRY:{driveAndSteer.joystickSteer(right_thumb_y)}\n".encode())
            if trigger_R:
                ser.write(f"triggerR:{trigger_R}\n".encode())
            if trigger_L:
                ser.write(f"triggerL:{trigger_L}\n".encode())

            

            time.sleep(0.1)  # Control loop speed

        except Exception as e:
            print(f"An error occurred: {e}")
            break

if __name__ == "__main__":
    main()
