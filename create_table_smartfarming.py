#!/usr/bin/env python3

# Save script on each RPI0W
# This script contain two part
#   1. Will create table if device/table_name not in database
#   2. Insert value to database
# need import module/write a script to transfer Arduino converted readings


import mysql.connector
#TODO import arduino_converted_reading

# create table named "IP/custom name"
table_name = "SmartFarm_Alpha"

# establish connection (SmartFarming database/mysql)
mydb = mysql.connector.connect(
  host="192.168.0.142", # IP address hosting the MySQL
  user="smartfarming",  # MySQL username
  password="smartfarming",  # MySQL pass
  database="SmartFarming"
)

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
""".format(table_name)

# execute the SQL command
mycursor.execute(sql)



# SQL query to INSERT VALUES into table
sensor_sql = """INSERT INTO `{}` VALUES (%s);""".format(table_name) # TODO: add sensors variable
sensor_values = ()

try:
  # execute the SQL command
  mycursor.execute(sensor_sql, sensor_values)

  # commit changes in database
  mydb.commit()

except:
  # Rolling back in case of error
  mydb.rollback()

mydb.close()