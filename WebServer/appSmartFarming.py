from flask import Flask, render_template, request
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
        # name, dt, ec, humidity, ph, temperature = i
        return i


app = Flask(__name__)

#main route
@app.route("/")
def index():
    device, date_time, ec, humidity, ph, temperature = last_data()
    templateData = {
        'device': device,
        'date_time': date_time,
        'ec': ec,
        'humidity': humidity,
        'ph': ph,
        'temperature': temperature
    }
    return render_template("index.html", **templateData)

if __name__=="__main__":
    app.run(host="0.0.0.0", port=80)