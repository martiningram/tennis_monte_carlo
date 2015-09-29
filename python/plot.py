import pandas as pd
import plotly.plotly as py
from plotly.graph_objs import *
import numpy as np
import matplotlib.pyplot as plt

def plot_iid_vs_non_iid(df):
    trace0 = Scatter(
        x = df["p_win_iid"],
        y = df["p_win_dynamic"],
        mode = 'markers',
        name = 'dynamic',
        text = df["Player 1"].map(str) + " vs " + df["Player 2"],
        marker=Marker(
            size=8,
            )
        )

    trace1 = Scatter(
        x = df["p_win_iid"],
        name = 'iid',
        y = df["p_win_iid"],
        mode = 'lines'
        )

    data = Data([trace0, trace1])

    layout = Layout(

        xaxis=XAxis(
            range = [0, 1],
            title = 'p_win_iid',
            autorange=False,
            ),

        yaxis=YAxis(
            range = [0, 1],
            title = 'p_win_dynamic',
            autorange = False,
            ),
        )

    fig = Figure(data=data, layout=layout)
    plot_url = py.plot(fig, filename = 'non-iid-effect')

df = pd.read_csv('iid_vs_non_iid.csv')

print(df)

# plot_iid_vs_non_iid(df)

# plt.scatter(df["p_win_iid"], df["p_win_dynamic"] - df["p_win_iid"])
# plt.show()
