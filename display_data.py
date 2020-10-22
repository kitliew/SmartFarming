import mysql.connector

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
mycursor.execute("SELECT * from JOE")

for i in mycursor:
    print(i)
