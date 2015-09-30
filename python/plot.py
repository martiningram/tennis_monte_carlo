import pandas as pd
import plotly.plotly as py
from plotly.graph_objs import *
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

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
    plot_url = py.plot(fig, filename = 'non-iid-effect-coin')

df = pd.read_csv('iid_vs_non_iid_fixed_coin.csv')

spw_1_iid = df["p1_spw_iid"]
spw_2_iid = df["p2_spw_iid"]

delta_spw = spw_1_iid - spw_2_iid
sum_spw = spw_1_iid + spw_2_iid
delta_pred = df["p_win_iid"] - df["p_win_dynamic"]

# Discretise:

# bins = np.linspace(min(delta_spw), max(delta_spw), 75)
# 
# xs, ys = list(), list()
# 
# for cur_bin, next_bin in zip(bins, bins[1:]):
# 
#     cur_x = (cur_bin + next_bin) / 2
# 
#     cur_ys = delta_pred[(delta_spw >= cur_bin) & (delta_spw < next_bin)]
# 
#     cur_y = np.average(cur_ys)
# 
#     xs.append(cur_x)
#     ys.append(cur_y)
# 
# plt.scatter(xs, ys)
# plt.show()

# plt.scatter(delta_spw, delta_pred)

# fig = plt.figure()
# ax = fig.add_subplot(111, projection='3d')
# ax.scatter(delta_spw, sum_spw, delta_pred)
# plt.show()

print(df.columns)

# plt.hist(df["average_games_iid"] - df["average_games"])
# plt.show()

# plot_iid_vs_non_iid(df)

plt.scatter(delta_spw, -delta_pred)
plt.xlabel('spw_1_iid - spw_2_iid')
plt.ylabel('p_win_dynamic - p_win_iid')
plt.show()

# plt.scatter(df["p_win_iid"], df["p_win_dynamic"] - df["p_win_iid"])
# plt.show()
