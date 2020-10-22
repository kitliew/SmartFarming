#!/usr/bin/env python3

# RPI0W Serial USB communication to send data from the Arduino to RPI0W
# Find port with Arduino connected
# Establish connection
# Read Serial data printed from Arduino
# Output tuple(Device, Date, Time, EC, Humidity, pH, Temperature)

from db_script import Farmer
import serial.tools.list_ports
import time
import os

# arduino parameter, assign serial device name if known
arduino_port = '/dev/ttyUSB0'  # usually ttyUSB* or ttyACM*
baud = 115200

if __name__ == "__main__":
    while not os.path.exists(arduino_port):
        # noinspection ProblematicWhitespace
        print(arduino_port + " not found. Please choose the following:")
        possible_arduino = serial.tools.list_ports.comports()
        for p in possible_arduino:
            print(p)
        arduino_port = '/dev/' + str(input("\nChoose which port: "))
        print(arduino_port + " chosen.")

    ser = serial.Serial(arduino_port, baud, timeout=2)
    ser.flush()

    # skip the first 4 lines printed from libraries
    for x in range(4):
        ser.readline().decode('utf-8').rstrip()

    while True:
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').rstrip().split( )

            #TODO remove the first few lines (_acidVoltage, neutralVoltage etc)

            # arduino output format "pH: 7.42 , EC: 0.00 ms/cm, Temperature: 25.69 °C"
            ph, ec, temperature = line[1::3]

            # Output tuple(Device, DateTime, EC, Humidity, pH, Temperature)
            test = Farmer("JOE")
            sensors = ("JOE1", time.strftime('%Y-%m-%d %H:%M:%S'), float(ec), 100, float(ph), float(temperature))
            test.insert_meter_values(sensors)
            #
            # test1 = Farmer("JOE2")
            # sensors = ("JOE2", date_now, time_now, 1, 2, 3, 28)
            # test1.insert_meter_values(sensors)

            #
            # test.mydb.close()


