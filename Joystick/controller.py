import xinput
import time
import serial

ser = serial.Serial('COM3', 500000, timeout=1)  #Adjust COM port and baud rate as necessary
deadzone = 0 #0.1 * 32767

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
    prev_x, prev_y = 0, 0

    while True:
        try:
            if ser.in_waiting > 0:
                arduino_data = ser.readline().decode('utf-8').rstrip()  #read data from Arduino, decoding from unicode, stripping to remove any unnecessary characters
                print(f"Received input from Arduino: {arduino_data}")
            
            controller_state = xinput.get_state(controller_id)
            buttons = controller_state.Gamepad.wButtons
            left_thumb_x, left_thumb_y, right_thumb_x, right_thumb_y = get_thumb_values(controller_state)
            trigger_R, trigger_L = get_trigger_values(controller_state)

            #Joystick events

            ###Drive
            print(f"Raw joystick values - X: {left_thumb_x}, Y: {left_thumb_y}")

            # Only send if left stick values changed
            if left_thumb_x != prev_x or left_thumb_y != prev_y:
                ser.write(f"stickLX&LY:{left_thumb_x},{left_thumb_y}\n".encode())
                prev_x, prev_y = left_thumb_x, left_thumb_y

            if buttons & xinput.BUTTON_A:
                ser.write("BUTTON_A\n".encode())

            time.sleep(0.3)  #Control loop speed

        except Exception as e:
            print(f"An error occurred: {e}")
            break

if __name__ == "__main__":
    main()
