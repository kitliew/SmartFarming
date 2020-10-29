import mysql.connector
import time

#TODO get data from mysql
#TODO graph dot plot
#TODO live update?



host = "192.168.0.142"  # IP address hosting the MySQL
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

mycursor = mydb.cursor()

mycursor.execute("USE SmartFarming")


def last_data():
    sql = "SELECT * FROM JOE ORDER BY DateTime DESC LIMIT 1;"
    mycursor.execute(sql)
    for i in mycursor:
        # return rows:
        # device, dt, ec, humidity, ph, temperature = i
        return i

name, dt, ec, humidity, ph, temperature = last_data()
print(dt)