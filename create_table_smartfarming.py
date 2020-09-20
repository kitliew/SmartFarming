#!/usr/bin/env python3

import mysql.connector
import sys


# create table named "IP/custom name"
table_name = "SmartFarm_Alpha"

# connect to mysql SmartFarming database
mydb = mysql.connector.connect(
  host="192.168.0.142", # IP address hosting the MySQL
  user="smartfarming",  # MySQL username
  password="smartfarming",  # MySQL pass
  database="SmartFarming"
)

# execute 2 command
# command 1 create table if not exists (DATE: YYYY-MM-DD, TIME: hh:mm:ss)
command1 = """CREATE TABLE IF NOT EXISTS `{}` (
Device TEXT,
Date DATE,
Time TIME,
EC FLOAT,
Humidity INT,
pH FLOAT,
Temperature FLOAT
);
""".format(table_name)
# command 2 insert/sensors value
command2 = """INSERT INTO `{}` VALUES ();""".format(table_name) # TODO: add sensors variable


mycursor = mydb.cursor()
mycursor.execute(command1)
print(mycursor)




sys.exit()
mycursor.execute(command1)


