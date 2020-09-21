#!/usr/bin/env python3

# Save script on each RPI0W
# This script contain two part
#   1. Will create table if device/table_name not in database
#   2. Insert value to database
# need import module/write a script to transfer Arduino converted readings

# arrange/assign class to code. Potential to use multiple Arduino for one RPI0W

import sys
import mysql.connector
#TODO import arduino_converted_reading

# global variable
host="192.168.0.142"    # IP address hosting the MySQL
user="smartfarming"   # MySQL username
password="smartfarming"   # MySQL password
database="SmartFarming"   # MySQL database name


class Farmer(object):

  host = host
  user = user
  password = password
  database = database

  # establish connection (SmartFarming database/mysql)
  mydb = mysql.connector.connect(
    host=host,  # IP address hosting the MySQL
    user=user,  # MySQL username
    password=password,  # MySQL pass
    database=database
  )

  def __init__(self, device):
    self.farmer = device    # will be used as table name

    # creating a cursor object using the cursor() method
    mycursor = mydb.cursor()

    # preparing SQL query to CREATE TABLE into the database.
    sql = """CREATE TABLE IF NOT EXISTS `{}` (
    Device TEXT,
    Date DATE,
    Time TIME,
    EC FLOAT,
    Humidity INT,
    pH FLOAT,
    Temperature FLOAT
    );
    """.format(self.farmer)

    # execute the SQL command
    mycursor.execute(sql)

    #TODO should i close the cursor?

  def insert_meter_values(self, sensors):
    # SQL query to INSERT VALUES into table
    sensor_sql = """INSERT INTO `{}` VALUES (%s);""".format(table_name) # TODO: add sensors variable

    # creating a cursor object using the cursor() method
    mycursor = mydb.cursor()

    try:
      # execute the SQL command
      mycursor.execute(sensor_sql, sensors)

      # commit changes in database
      mydb.commit()

    except:
      # Rolling back in case of error
      mydb.rollback()

    mydb.close()