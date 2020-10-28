import dash
import dash_core_components as dcc
import dash_html_components as html
from dash.dependencies import Input, Output
import plotly.express as px
import plotly.graph_objects as go
import pandas as pd
import mysql.connector

app = dash.Dash(__name__)

# -----------------------------------------------------------------------------------------------
# Import and clean data (import mysql into pandas)
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

def getsqlData():
    sql = "SELECT * FROM JOE ORDER BY DateTime DESC "
    mycursor.execute(sql)
    data = mycursor.fetchall()
    df = pd.DataFrame(data, columns=["Name", "DateTime", "EC", "Humidity", "pH", "Temperature"])
    return df


# -----------------------------------------------------------------------------------------------
# App layout
app.layout = html.Div([

    html.H1("SmartFarming Dashboards", style={"text-align": "center"}),

    dcc.Dropdown(id="slct_month",
                 options=[
                     {"label": "JAN", "value": 1},
                     {"label": "FEB", "value": 2},
                     {"label": "MAR", "value": 3},
                     {"label": "APR", "value": 4},
                     {"label": "MAY", "value": 5},
                     {"label": "JUN", "value": 6},
                     {"label": "JUL", "value": 7},
                     {"label": "AUG", "value": 8},
                     {"label": "SEP", "value": 9},
                     {"label": "OCT", "value": 10},
                     {"label": "NOV", "value": 11},
                     {"label": "DEC", "value": 12}],
                 multi=False,
                 value=10,
                 style={"width":"30%", "display": "inline-block"}
                 ),
    dcc.Dropdown(id="slct_sensor",
                 options=[
                     {"label": "EC", "value": "EC"},
                     {"label": "Humidity", "value": "Humidity"},
                     {"label": "pH", "value": "pH"},
                     {"label": "Temperature", "value": "Temperature"}],
                 multi=False,
                 value="Temperature",
                 style={"width":"30%", "display": "inline-block"}
                 ),
    dcc.Graph(id="env_graph")

    ])


# -----------------------------------------------------------------------------------------------
# Connect the Plotly graphs with Dash Components
@app.callback(
    Output(component_id="env_graph", component_property="figure"),
    [Input(component_id="slct_month", component_property="value"),
     Input(component_id="slct_sensor", component_property="value")]
)
def update_graph(option_slctd_month, option_slctd_sensor):

    container = "The month chosen by user was: {}".format(option_slctd_month)

    dff = getsqlData()
    dff["Month"] = pd.DatetimeIndex(dff["DateTime"]).month

    dff = dff[dff["Month"] == option_slctd_month]

    # Plotly Express
    fig = px.scatter(dff, x=dff["DateTime"], y=dff[option_slctd_sensor], color=dff["Name"], trendline="lowess")
    return fig


# -----------------------------------------------------------------------------------------------
# Run App
app.run_server(debug=True)