import serial
import time

ser = serial.Serial('COM6', 9600)
time.sleep(2)  


try:
    message = "Hello "
    ser.write(message.encode())
    
    response = ser.readline().decode()
    print("Response:", response)
finally:
    
    ser.close()