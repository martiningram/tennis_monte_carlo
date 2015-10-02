import pandas as pd
# import plotly.plotly as py
# from plotly.graph_objs import *
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

def plot_iid_vs_non_iid(df):
    trace0 = Scatter(
        x = df["p_win_iid"],
        y = df["p_win_dynamic"],
        mode = 'markers',
        name = 'dynamic_imp',
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

def plot_iid_vs_non_iid_pyplot(df):

    iid_prob = df["p_win_iid"]
    non_iid_prob = df["p_win_dynamic"]

    plt.scatter(iid_prob, non_iid_prob)
    plt.hold('on')
    plt.plot(iid_prob, iid_prob)
    plt.show()

def explore_detailed_data():
    df = pd.read_csv('match_one.csv')

    df = df.dropna()

    p1 = df[df["serving"] == "Adrian Mannarino"]

    deviations = p1["p_noniid"] - p1["p_iid"]

    print(p1.columns)
    print(p1["serve_points_won"].unique())

    only_tb = p1[p1["tiebreak"] == True]

    print("TB analysis:")

    print(only_tb["serve_points_won"].unique())

    print(min((p1[p1["serve_points_won"] == 0]).p_noniid))

    print(p1[(p1["serving_games_won"] == 0) & (p1["returning_games_won"] == 5) & \
             (p1["serving_sets_won"] == 2) & (p1["returning_sets_won"] == 0) &
             (p1["serve_points_won"] == 3) & (p1["return_points_won"] == 2)])

def by_ranking(df, df_results):
    print(df.iloc[1])
    print(df_results.iloc[1])

    for i,mc_row in df.iterrows():
        # Find rankings:
        matching_results = \
        df_results[(df_results["serving"] == mc_row["Player 1"]) & \
                   (df_results["returning"] == mc_row["Player 2"])]
        print(matching_results.shape)

df = pd.read_csv('../iid_vs_non_iid_players.csv')
df_results = pd.read_csv('../atp_points_predicted_player.csv')

by_ranking(df, df_results)
assert(False)

# # Discretise:

# bins = np.linspace(min(delta_spw), max(delta_spw), 50)

# xs, ys = list(), list()

# for cur_bin, next_bin in zip(bins, bins[1:]):

#     cur_x = (cur_bin + next_bin) / 2

#     cur_ys = delta_pred[(delta_spw >= cur_bin) & (delta_spw < next_bin)]

#     cur_y = np.average(cur_ys)

#     xs.append(cur_x)
#     ys.append(cur_y)

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

# plot_iid_vs_non_iid_pyplot(df)

# plt.scatter(delta_spw, -delta_pred)
# plt.xlabel('spw_1_iid - spw_2_iid')
# plt.ylabel('p_win_dynamic - p_win_iid')
# plt.show()

# plt.scatter(df["p_win_iid"], df["p_win_dynamic"] - df["p_win_iid"])
# plt.show()
