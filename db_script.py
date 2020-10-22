#!/usr/bin/env python3

# Save script on each RPI0W
# This script contain two part
#   1. Will create table if device/table_name not in database
#   2. Insert value to database


import mysql.connector


class Farmer(object):
    # global variable
    host = "192.168.0.142"  # IP address hosting the MySQL  #TODO CHANGE IP
    user = "smartfarming"  # MySQL username
    password = "smartfarming"  # MySQL password
    database = "SmartFarming"  # MySQL database name

    # establish connection (SmartFarming database/mysql)
    mydb = mysql.connector.connect(
        host=host,  # IP address hosting the MySQL
        user=user,  # MySQL username
        password=password,  # MySQL pass
        database=database
    )

    def __init__(self, device):
        self.farmer = device  # will be used as table name

        # creating a cursor object using the cursor() method
        mycursor = self.mydb.cursor()

        # preparing SQL query to CREATE TABLE into the database.
        sql = """CREATE TABLE IF NOT EXISTS `{}` (
            Device TEXT,
            DateTime DATETIME,
            EC FLOAT,
            Humidity INT,
            pH FLOAT,
            Temperature FLOAT
            );
            """.format(self.farmer)

        # execute the SQL command
        mycursor.execute(sql)

    def insert_meter_values(self, sensors):
        # SQL query to INSERT VALUES into table
        sensor_sql = """INSERT INTO `{}` VALUES {};""".format(self.farmer, sensors)

        # creating a cursor object using the cursor() method
        mycursor = self.mydb.cursor()

        try:
            # execute the SQL command
            mycursor.execute(sensor_sql)

            # commit changes in database
            self.mydb.commit()
            print("Commit sensors reading {}".format(sensors))

        except:
            # Rolling back in case of error
            self.mydb.rollback()
            print("error, rollback")

        # Do not close connection here. Will terminate other connection
        # self.mydb.close()
        # print("CLOSED")
