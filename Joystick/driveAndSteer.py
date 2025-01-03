def joystickSteer(joystick_value):
    scale_Max_Min_Val = 180
    if joystick_value > 0:
        #Forward motion: 0 to 32767 --> 1500 to 2500
        angle = 180 + (joystick_value / 32767) * scale_Max_Min_Val  # 1000 is the range from 1500 to 2500
    elif joystick_value < 0:
        #Backward motion: -32768 to 0 --> 500 to 1500
        angle = 180 + (joystick_value / -32767) * -(scale_Max_Min_Val)  # -1000 is the range from 1500 to 500
    else:
        #Neutral position
        angle = 180

    print(f"Joystick Value: {joystick_value} -> Angle: {int(angle)}")
    return int(angle)


def joystickDrive(joystick_value):
    scale_Max_Min_Val = 500
    if joystick_value > 0:
        #Forward motion: 0 to 32767 --> 1500 to 2500
        pwm_value = 1500 + (joystick_value / 32767) * scale_Max_Min_Val  # 1000 is the range from 1500 to 2500
    elif joystick_value < 0:
        #Backward motion: -32768 to 0 --> 500 to 1500
        pwm_value = 1500 + (joystick_value / -32768) * -(scale_Max_Min_Val)  # -1000 is the range from 1500 to 500
    else:
        #Neutral position
        pwm_value = 1500

    print(f"Joystick Value: {joystick_value} -> PWM Value: {int(pwm_value)}")
    return int(pwm_value)
