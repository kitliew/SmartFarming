#!/usr/bin/env python3

# RPI0W Serial USB communication to send data from the Arduino to RPI0W
# Auto choose port with Arduino connected
# Establish connection
# Read Serial data printed from Arduino
# Output tuple(Device, Date, Time, EC, Humidity, pH, Temperature)
# -----------------------------------------------------------------------------

from db_script import Farmer
import serial.tools.list_ports
import time
import os

# arduino parameter, assign serial device name if known
arduino_port = '/dev/ttyUSB0'  # usually ttyUSB* or ttyACM*
baud = 115200


if __name__ == "__main__":

    # Check if port exist
    if not os.path.exists(arduino_port):

        # Find possible port, usually ttyUSB* or ttyACM*
        ports = [port.device for port in serial.tools.list_ports.comports() if ("ttyUSB" in port.device) or ("ttyACM" in port.device)]
        # Choose first port
        arduino_port = ports[0]

    # Establish serial connection
    ser = serial.Serial(arduino_port, baud, timeout=2)
    ser.flush()

    # Skip the first 4 lines printed from libraries (_acidVoltage, neutralVoltage etc)
    for x in range(4):
        ser.readline().decode('utf-8').rstrip()


# -------------------------------------------------------------------------------------------------------------------------------------
# Get readings

    while True:

        # If reading is waiting
        if ser.in_waiting > 0:

            # Decode readings from Arduino (utf-8)
            line = ser.readline().decode('utf-8').rstrip().split()

            #TODO add humidity sensor

            # arduino output format "pH: 7.42 , EC: 0.00 ms/cm, Temperature: 25.69 °C"
            ph, ec, temperature = line[1::3]

            # Output tuple(Device, DateTime, EC, Humidity, pH, Temperature)
            test = Farmer("JOE")
            sensors = ("JOE1", time.strftime('%Y-%m-%d %H:%M:%S'), float(ec), 100, float(ph), float(temperature))
            test.insert_meter_values(sensors)
