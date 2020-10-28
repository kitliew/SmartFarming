from matplotlib.backends.backend_agg import FigureCanvasAgg as FigureCanvas
from matplotlib.figure import Figure
import io
from flask import Flask, render_template, send_file, make_response, request
import mysql.connector
import time
app = Flask(__name__)

#TODO get data from mysql
#TODO graph dot plot
#TODO live update?

host = "192.168.0.143"  # IP address hosting the MySQL  #TODO change ip address
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

def getLastdata():
    sql = "SELECT * FROM JOE ORDER BY DateTime DESC LIMIT 1"
    mycursor.execute(sql)
    for i in mycursor:
        # name, dt, ec, humidity, ph, temperature = i
        return i

def getHistData(numSamples):
    sql = "SELECT * FROM JOE ORDER BY DateTime DESC LIMIT " + str(numSamples)
    mycursor.execute(sql)
    data = mycursor.fetchall()
    dt=[]
    ec=[]
    humidity=[]
    ph=[]
    temperature=[]
    for row in reversed(data):
        dt.append(row[1])
        ec.append(row[2])
        humidity.append(row[3])
        ph.append(row[4])
        temperature.append(row[5])
    return dt, ec, humidity, ph, temperature

def maxRowsTable():
    """count maximum number of rows in table"""
    #using any column for max number
    sql = "select COUNT(DateTime) from JOE"
    mycursor.execute(sql)
    for row in mycursor:
        print(row)
        return row[0]

# define and initialize global variables
global numSamples
numSamples = maxRowsTable()
if (numSamples > 101):
    numSamples = 100


#main route
@app.route("/")
def index():
    device, date_time, ec, humidity, ph, temperature = getLastdata()
    templateData = {
        'device': device,
        'date_time': date_time,
        'ec': ec,
        'humidity': humidity,
        'ph': ph,
        'temperature': temperature,
        'numSamples': numSamples
    }
    return render_template("index.html", **templateData)

@app.route("/", methods=["POST"])
def my_form_post():
    global numSamples
    numSamples = int(request.form['numSamples'])
    numMaxSamples = maxRowsTable()
    if (numSamples > numMaxSamples):
        numSamples = (numMaxSamples - 1)
    device, date_time, ec, humidity, ph, temperature = getLastdata()
    templateData = {
        'device': device,
        'date_time': date_time,
        'ec': ec,
        'humidity': humidity,
        'ph': ph,
        'temperature': temperature,
        'numSamples': numSamples
    }
    return render_template("index.html", **templateData)

@app.route('/plot/ec')
def plot_ec():
    dt, ec, humidity, ph, temperature = getHistData(numSamples)
    ys = ph
    xs = ec
    fig = Figure()
    axis = fig.add_subplot(1, 1, 1)
    axis.set_title("EC")
    axis.set_xlabel("DateTime")
    axis.grid(True)
    axis.plot(xs, ys)
    canvas = FigureCanvas(fig)
    output = io.BytesIO()
    canvas.print_png(output)
    response = make_response(output.getvalue())
    response.mimetype = "image/png"
    return response

@app.route('/plot/humidity')
def plot_humidity():
    dt, ec, humidity, ph, temperature = getHistData(numSamples)
    ys = ph
    xs = humidity
    fig = Figure()
    axis = fig.add_subplot(1, 1, 1)
    axis.set_title("Humidity [%]")
    axis.set_xlabel("DateTime")
    axis.grid(True)
    axis.plot(xs, ys)
    canvas = FigureCanvas(fig)
    output = io.BytesIO()
    canvas.print_png(output)
    response = make_response(output.getvalue())
    response.mimetype = "image/png"
    return response

@app.route('/plot/ph')
def plot_ph():
    dt, ec, humidity, ph, temperature = getHistData(numSamples)
    ys= ph
    xs = dt
    fig = Figure()
    axis = fig.add_subplot(1,1,1)
    axis.set_title("pH")
    axis.set_xlabel("DateTime")
    axis.grid(True)
    axis.plot(xs,ys)
    canvas = FigureCanvas(fig)
    output = io.BytesIO()
    canvas.print_png(output)
    response = make_response(output.getvalue())
    response.mimetype = "image/png"
    return response

@app.route('/plot/temperature')
def plot_temperature():
    dt, ec, humidity, ph, temperature = getHistData(numSamples)
    ys= temperature
    xs = dt
    fig = Figure()
    axis = fig.add_subplot(1,1,1)
    axis.set_title("Temperature [°C]")
    axis.set_xlabel("DateTime")
    axis.grid(True)
    axis.plot(xs,ys)
    canvas=FigureCanvas(fig)
    output=io.BytesIO()
    canvas.print_png(output)
    response=make_response(output.getvalue())
    response.mimetype="image/png"
    return response


if __name__=="__main__":
    app.run(host="0.0.0.0", port=80)