import dash
import dash_core_components as dcc
import dash_html_components as html
from dash.dependencies import Input, Output
import plotly.express as px
import plotly.graph_objects as go
import pandas as pd
import mysql.connector

#TODO https://dash-docs.herokuapp.com/dash-core-components
#TODO dcc.DatePickerRange

app = dash.Dash(__name__)

# -----------------------------------------------------------------------------------------------
# Import and clean data (import mysql into pandas)
host = "192.168.0.142"  # IP address hosting the MySQL  #TODO change ip address
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

#TODO define function in another file, import them
def getsqlData():
    sql = "SELECT * FROM JOE ORDER BY DateTime DESC "
    mycursor.execute(sql)
    data = mycursor.fetchall()
    df = pd.DataFrame(data, columns=["Name", "DateTime", "EC", "Humidity", "pH", "Temperature"])
    return df

def get24hsqlData():
    sql = """SELECT * FROM `JOE` WHERE 
        DateTime >= NOW() - INTERVAL 1 DAY
        ORDER BY DateTime DESC;"""
    mycursor.execute(sql)
    data = mycursor.fetchall()
    df = pd.DataFrame(data, columns=["Name", "DateTime", "EC", "Humidity", "pH", "Temperature"])
    return df

def df24h_min_max(sensor):
    df = get24hsqlData()
    high = df[str(sensor)].max()
    low = df[str(sensor)].min()
    current = df[str(sensor)].iloc[0]
    return low, current, high


# -----------------------------------------------------------------------------------------------
# App layout
app.layout = html.Div(
    children=[
        html.Div(
            className="row",
            children=[
                # Column for icons (homepage, settings etc)
                html.Div(
                    className="two columns div-sidebar-menu",
                    children=[
                        html.H2("Home"),
                        html.H2("Settings"),
                        html.H2("Others")
                    ]
                ),
                # Column for app grids and graph
                html.Div(
                    className="ten columns div-for-sensors",
                    children=[
                        dcc.Dropdown(
                            id="slct_month",
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
                            value=10
                        ),
                        dcc.Tabs(id="sensors_graph_tabs", value="Temperature", children=[
                            dcc.Tab(label="EC", value="EC"),
                            dcc.Tab(label="Humidity", value="Humidity"),
                            dcc.Tab(label="pH", value="pH"),
                            dcc.Tab(label="Temperature", value="Temperature")
                        ]),
                        dcc.Graph(id="tab_content_graph"),
                    ]
                )
            ]
        )
    ]
)

# -----------------------------------------------------------------------------------------------
# Connect the Plotly graphs with Dash Components

# Graph for tabs
@app.callback(
    Output(component_id="tab_content_graph", component_property="figure"),
    [Input(component_id="slct_month", component_property="value"),
     Input(component_id="sensors_graph_tabs", component_property="value")],
)
def update_graph(option_slctd_month, option_slctd_sensor):


    dff = getsqlData()
    dff["Month"] = pd.DatetimeIndex(dff["DateTime"]).month

    dff = dff[dff["Month"] == option_slctd_month]

    # Plotly Express
    fig = px.scatter(dff, x=dff["DateTime"], y=dff[option_slctd_sensor], color=dff["Name"])
    return fig


# -----------------------------------------------------------------------------------------------
# Run App
app.run_server(debug=True)