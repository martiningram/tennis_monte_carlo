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

    results = list()
    processed_matches = list()

    print(df_results.columns)

    for i,mc_row in df.iterrows():
        # Find rankings:
        matching_results = \
        df_results[(df_results["serving"] == mc_row["Player 1"]) & \
                   (df_results["returning"] == mc_row["Player 2"])]

        for i,res_row in matching_results.iterrows():
            if res_row["serving_match"][:len(mc_row["Match title"])] == \
            mc_row["Match title"] \
            and res_row["serving_match"] not in processed_matches:
                higher_ranked = res_row["server_rank"] \
                                if res_row["server_rank"] < res_row["return_rank"] \
                                else res_row["return_rank"]
                lower_ranked = res_row["server_rank"] \
                               if res_row["server_rank"] > res_row["return_rank"] \
                               else res_row["return_rank"]

                [higher_ranked_iid, higher_ranked_dynamic] = \
                [mc_row["p_win_iid"], mc_row["p_win_dynamic"]] \
                if mc_row["Player 1"] == higher_ranked \
                else [1 - mc_row["p_win_iid"], 1 - mc_row["p_win_dynamic"]]

                cur_results = {"p1_rank": res_row["server_rank"],
                               "p2_rank": res_row["return_rank"],
                               "tournament" : res_row["tournament"],
                               "p1_won" : res_row["server_win"],
                               "higher_ranked" : higher_ranked,
                               "lower_ranked" : lower_ranked,
                               "higher_ranked_iid" : higher_ranked_iid,
                               "higher_ranked_dynamic" : higher_ranked_dynamic,
                               "delta_p" : mc_row["p_win_dynamic"] - \
                mc_row["p_win_iid"]}

                results.append(cur_results)
                processed_matches.append(res_row["serving_match"])

    matched_rankings = pd.DataFrame(results, index = df.index)
    result = pd.concat([df, matched_rankings], axis=1)

    wrong_ones = [x for i,x in result.iterrows() \
                  if x["p_win_dynamic"] > 0.5 and x["p1_won"] == 0]

    print(wrong_ones)
    assert(False)

    brier = np.sum((result["p_win_dynamic"] - result["p1_won"])**2) / \
            (float(result.shape[0]))
    brier_iid = np.sum((result["p_win_iid"] - result["p1_won"])**2) / \
                (float(result.shape[0]))

    print(result.groupby('Player 1').mean().sort('delta_p'))

    assert(False)

    result.to_csv("matched_simulations.csv")

    print(brier, brier_iid)

df = pd.read_csv('../iid_vs_non_iid_players.csv')
df_results = pd.read_csv('../atp_points_predicted_player.csv')

by_ranking(df, df_results)

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

plt.hist(df["p_win_dynamic"] - df["p_win_iid"], 50)
plt.xlabel("Difference in winning probability, dynamic model")
plt.ylabel("Frequency in dataset")
plt.show()

# plt.hist(df["average_sets"] - df["average_sets_iid"], 50)
# plt.show()

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
