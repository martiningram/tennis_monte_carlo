import pandas as pd
import plotly.plotly as py
from plotly.graph_objs import *
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import datetime

# Given a list of labels and points (xs, ys), plots a scatter plot and labels
# each point. plot_line optionally draws a linear fit through the points.
def plot_annotated_scatter(labels, xs, ys, plot_line = False):

    plt.subplots_adjust(bottom = 0.1)

    plt.scatter(xs, ys, marker = 'o')

    for label, x, y in zip(labels, xs, ys):
        plt.annotate(
            label, 
            xy = (x, y), xytext = (-10, 10),
            textcoords = 'offset points', ha = 'right', va = 'bottom',
            bbox = dict(boxstyle = 'round,pad=0.3', fc = 'yellow', alpha = 0.5),
            arrowprops = dict(arrowstyle = '->', connectionstyle =
                              'arc3,rad=0'),
            fontsize = 8)

    if (plot_line):
        coeff = np.polyfit(xs,ys,1)
        plt.plot(xs,np.polyval(coeff,xs))

    return plt

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

def plot_average_deviation(df):

    delta_spw = df["p_win_dynamic"] - df["p_win_iid"]
    bins = np.linspace(min(delta_spw), max(delta_spw), 50)
    xs, ys = list(), list()

    for cur_bin, next_bin in zip(bins, bins[1:]):

        cur_x = (cur_bin + next_bin) / 2
        cur_ys = delta_pred[(delta_spw >= cur_bin) & (delta_spw < next_bin)]
        cur_y = np.average(cur_ys)

        xs.append(cur_x)
        ys.append(cur_y)

    plt.scatter(xs, ys)
    plt.show()

def add_ranking(df, df_results):

    print(df.columns)
    print(df_results.columns)

    results = list()
    processed_matches = list()

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
                if res_row["server_rank"] == higher_ranked \
                else [1 - mc_row["p_win_iid"], 1 - mc_row["p_win_dynamic"]]

                cur_results = {"p1_rank": res_row["server_rank"],
                               "p2_rank": res_row["return_rank"],
                               "tournament" : res_row["tournament"],
                               "p1_won" : res_row["server_win"],
                               "higher_ranked" : higher_ranked,
                               "lower_ranked" : lower_ranked,
                               "higher_ranked_iid" : higher_ranked_iid,
                               "higher_ranked_dynamic" : higher_ranked_dynamic,
                               "higher_ranked_won" : 1 if (res_row["server_win"] == 1 and \
                               res_row["server_rank"] == higher_ranked) or  \
                               (res_row["server_win"] == 0 and \
                                res_row["return_rank"] == higher_ranked) \
                               else 0,
                               "delta_p" : mc_row["p_win_dynamic"] - \
                mc_row["p_win_iid"]}

                results.append(cur_results)
                processed_matches.append(res_row["serving_match"])

    matched_rankings = pd.DataFrame(results, index = df.index)
    result = pd.concat([df, matched_rankings], axis=1)
    print(result.columns)
    result.to_csv("matched_simulations.csv")

    entries = result.shape[0]

    brier_iid = 1/float(entries) * np.sum((result["p_win_iid"] - result["p1_won"])**2)
    brier_dynamic = 1/float(entries) * \
                    np.sum((result["p_win_dynamic"] - result["p1_won"])**2)

    print(brier_iid, brier_dynamic)
    
    return result

def plot_deviation_histogram(df):
   plt.hist(df["p_win_dynamic"] - df["p_win_iid"], 50)
   plt.xlabel("Difference in winning probability, dynamic model")
   plt.ylabel("Frequency in dataset")
   print(df[(df["p_win_dynamic"] - df["p_win_iid"]) == \
            np.max(df["p_win_dynamic"] - df["p_win_iid"])])
   plt.show()

def plot_by_ranking(df):
    min_bin = np.linspace(1,100,10)
    print(min_bin)

    results = list()

    for (lower_lim, upper_lim) in zip(min_bin,min_bin[1:]):

        matching_rows = df[(df["higher_ranked"] > lower_lim) \
                           & (df["higher_ranked"] <= upper_lim)]

        pred_prob_dyn = np.median(matching_rows["higher_ranked_dynamic"])
        pred_prob_iid = np.median(matching_rows["higher_ranked_iid"])
        av_win = np.average(matching_rows["higher_ranked_won"])

        results.append([lower_lim, upper_lim, pred_prob_iid, pred_prob_dyn, av_win])

    results = np.transpose(np.array(results))

    plt.bar((results[0,:]+results[1,:]) / 2, results[2,:], label="iid", color='g')
    plt.bar((results[0,:]+results[1,:]) / 2 + 1, results[3,:], label="dynamic")
    plt.bar((results[0,:]+results[1,:]) / 2 + 2, results[4,:], label="actual", color='r')
    plt.xlabel('Ranking, higher-ranked player (binned)')
    plt.ylabel('Average probability of winning')
    plt.title('ATP')
    plt.legend()
    plt.show()

def find_correct_bumps(df):

    delta = df["delta_p"]

    correct_bumps = np.sum(((delta < 0) & (df["p1_won"] == 0)) | \
                    ((delta > 0) & (df["p1_won"] == 1)))

    incorrect_bumps = delta.shape[0] - correct_bumps

    print(correct_bumps / float(delta.shape[0]), incorrect_bumps / float(delta.shape[0]))

def plot_average_offset(df):

    names = list()
    av_offsets = list()
    spw_deltas = list()
    spws = list()

    for player in pd.concat([df["Player 1"], df["Player 2"]]).unique():

        if (player == "marcel granollers"):
            continue

        p1_matches = df[df["Player 1"] == player]
        offsets_as_p1 = p1_matches["delta_p"]
        iid_prob_p1 = p1_matches["p_win_iid"]
        spw_p1 = p1_matches["p1_spw_iid"]

        p2_matches = df[df["Player 2"] == player]
        offsets_as_p2 = -p2_matches["delta_p"]
        iid_prob_p2 = 1 - p2_matches["p_win_iid"]
        spw_p2 = p2_matches["p2_spw_iid"]

        if (pd.concat([p1_matches, p2_matches]).shape[0] > 5):

            names.append(player)
            av_offsets.append(np.median(pd.concat([offsets_as_p1, offsets_as_p2])))
            spw_deltas.append(np.median(pd.concat([iid_prob_p1, iid_prob_p2])))
            spws.append(np.median(pd.concat([spw_p1, spw_p2])))

    plt = plot_annotated_scatter(names, spw_deltas, av_offsets)
    plt.xlabel("Median i.i.d. winning probability (skill)")
    plt.ylabel("Median dynamic correction (mentality)")

    plt.figure(2)
    plt = plot_annotated_scatter(names, spws, av_offsets)
    plt.show()

def check_predictions(predictions_file, tournament, year, t_type):

    save_name = "predictions " + tournament + " " + str(year) + ".csv"

    if t_type != "atp":
        print("Only atp match data available currently")
        exit()

    if year == 2014:
        match_file = "matches_2014_atp.csv"

    elif year == 2015:
        match_file = "matches_loop_2015.csv"

    df = pd.read_csv(predictions_file)
    data = pd.read_csv(match_file)

    results = list()

    # Match rows:

    for i,row in df.iterrows():

        cur_p1 = row["Player 1"]
        cur_p2 = row["Player 2"]

        print(cur_p1, cur_p2)

        cur_data = data[data["event_name"].str.lower().str.contains(tournament)]

        # Match the current match

        cur_match_data = cur_data[((cur_data["playerA"].str.lower() == cur_p1.lower()) & \
                                   (cur_data["playerB"].str.lower() == cur_p2.lower()) | \
                                   ((cur_data["playerA"].str.lower() == cur_p2.lower()) & \
                                    (cur_data["playerB"].str.lower() == cur_p1.lower())))]

        if (cur_match_data.shape[0] == 1):

            cur_match_data = cur_match_data.iloc[0]

            score = cur_match_data["score"]

            player_a = cur_match_data["playerA"]
            player_b = cur_match_data["playerB"]

            # Calculate SPW:

            fsp_a = cur_match_data["a_first_serve_hits"] / \
                    float(cur_match_data["a_first_serve_total"])
            fsw_a = cur_match_data["a_first_serve_won"] / \
                    float(cur_match_data["a_first_serve_hits"])
            ssw_a = cur_match_data["a_second_serve_won"] / \
                    float(cur_match_data["a_second_serve_played"])

            iid_a = fsp_a * fsw_a + (1 - fsp_a) * ssw_a

            fsp_b = cur_match_data["b_first_serve_hits"] / \
                    float(cur_match_data["b_first_serve_total"])
            fsw_b = cur_match_data["b_first_serve_won"] / \
                    float(cur_match_data["b_first_serve_hits"])
            ssw_b = cur_match_data["b_second_serve_won"] / \
                    float(cur_match_data["b_second_serve_played"])

            iid_b = fsp_b * fsw_b + (1 - fsp_b) * ssw_b

            p1_average_iid = iid_a if cur_match_data["playerA"].lower() == \
                             row["Player 1"].lower() else iid_b
            p2_average_iid = iid_b if cur_match_data["playerA"].lower() == \
                             row["Player 1"].lower() else iid_a

            p1_won = (cur_match_data["winner"].lower() == (row["Player 1"].lower()))

            print(p1_average_iid, p2_average_iid, row["p1_iid_spw"], row["p2_iid_spw"])

            results.append({"p1_won" : p1_won,
                            "actual_spw_p1" : p1_average_iid,
                            "actual_spw_p2" : p2_average_iid,
                            "p1_spw_predicted" : row["p1_iid_spw"],
                            "p2_spw_predicted" : row["p2_iid_spw"],
                            "Player 1" : row["Player 1"],
                            "Player 2" : row["Player 2"],
                            "p1_prob" : row["p1_win_prob"]})
            

            if "RET" in score:
                continue

    results = pd.DataFrame(results)

    brier = np.sum((results["p1_won"] - results["p1_prob"])**2) / results.shape[0]
    print(brier)

    if (results.shape[0] == 0):
        print("No matches found.")
        exit()

    # Calculate overall difference

    offsets_p1 = results["p1_spw_predicted"] - results["actual_spw_p1"]
    offsets_p2 = results["p2_spw_predicted"] - results["actual_spw_p2"]
    all_offsets = np.concatenate([offsets_p1.values, offsets_p2.values])

    bias, variance = np.average(all_offsets), np.std(all_offsets)

    pct = lambda x: str(round(x * 100, 1)) + "%"

    print(bias, variance)

    plt.hist(all_offsets, 20)
    plt.xlabel("Absolute deviation from match average spw")
    plt.ylabel("Frequency")
    plt.title("spw offsets, " + tournament + " " + str(year) + ". Brier: " + \
              str(round(brier,3)) + ", bias: " + pct(bias) + \
              ", variance: " + pct(variance))
    plt.savefig("Deviation histogram, " + tournament + " " + str(year) + ".png")
    plt.close()

    # Calculate delta distributions

    deltas_predicted = results["p1_spw_predicted"] - results["p2_spw_predicted"]
    deltas_actual = results["actual_spw_p1"] - results["actual_spw_p2"]

    good_ones = (deltas_predicted * deltas_actual > 0)
    bad_ones = ~good_ones

    names = results["Player 1"] + " " + results["Player 2"]

    plot = plot_annotated_scatter(names[good_ones],
                                  deltas_predicted[good_ones],
                                  deltas_actual[good_ones])

    plot.title(tournament + " " + str(year) + " expected")
    plot.xlabel("Expected advantage, player 1")
    plot.ylabel("Match advantage, player 1")
    plot.savefig(tournament + " " + str(year) + " expected.png")
    plot.close()

    plot = plot_annotated_scatter(names[bad_ones],
                                  deltas_predicted[bad_ones],
                                  deltas_actual[bad_ones])

    plot.title(tournament + " " + str(year) + " surprises")
    plot.xlabel("Expected advantage, player 1")
    plot.ylabel("Match advantage, player 1")
    plot.savefig(tournament + " " + str(year) + " surprises.png")
    plot.close()

    results.to_csv(save_name)

def add_atp_data(filename, df):

    data = pd.read_csv(filename)
    print(data.columns)

    results = list()

    # Match rows:

    for i,row in df.iterrows():

        cur_p1 = row["Player 1"]
        cur_p2 = row["Player 2"]

        cur_tournament = row["tournament"].lower()

        cur_tournament = "roland garros" if cur_tournament == "french open" \
                         else cur_tournament

        cur_data = data[data["event_name"].str.lower().str.contains(cur_tournament)]

        # Match the current match

        cur_match_data = cur_data[((cur_data["playerA"].str.lower() == cur_p1.lower()) & \
                                   (cur_data["playerB"].str.lower() == cur_p2.lower()) | \
                                   ((cur_data["playerA"].str.lower() == cur_p2.lower()) & \
                                    (cur_data["playerB"].str.lower() == cur_p1.lower())))]

        if (cur_match_data.shape[0] == 1):

            cur_match_data = cur_match_data.iloc[0]

            score = cur_match_data["score"]

            player_a = cur_match_data["playerA"]
            player_b = cur_match_data["playerB"]

            fsp_a = cur_match_data["a_first_serve_hits"] / \
                    float(cur_match_data["a_first_serve_total"])
            fsw_a = cur_match_data["a_first_serve_won"] / \
                    float(cur_match_data["a_first_serve_hits"])
            ssw_a = cur_match_data["a_second_serve_won"] / \
                    float(cur_match_data["a_second_serve_played"])

            iid_a = fsp_a * fsw_a + (1 - fsp_a) * ssw_a

            fsp_b = cur_match_data["b_first_serve_hits"] / \
                    float(cur_match_data["b_first_serve_total"])
            fsw_b = cur_match_data["b_first_serve_won"] / \
                    float(cur_match_data["b_first_serve_hits"])
            ssw_b = cur_match_data["b_second_serve_won"] / \
                    float(cur_match_data["b_second_serve_played"])

            iid_b = fsp_b * fsw_b + (1 - fsp_b) * ssw_b

            p1_average_iid = iid_a if cur_match_data["playerA"].lower() == \
                             row["Player 1"].lower() else iid_b
            p2_average_iid = iid_b if cur_match_data["playerA"].lower() == \
                             row["Player 1"].lower() else iid_a

            print(p1_average_iid, p2_average_iid, row["p1_spw_iid"], row["p2_spw_iid"])

            if "RET" in score:
                continue

            # split sets by spaces

            split_score = score.split(" ")

            games = 0

            for cur_set in split_score:

                split_cur_set = cur_set.split("-")

                p1_games = int(split_cur_set[0])

                p2_score = split_cur_set[1]

                p2_score_split = p2_score.split('(')[0]
                p2_score_split = p2_score_split.split(';')[0]

                p2_games = int(p2_score_split)

                games += p1_games + p2_games

            cur_results = {"p1": cur_p1, "p2": cur_p2, "score": score, "games":
                           games, "dynamic_games": row["average_games"],
                           "iid_games": row["average_games_iid"], "p1_spw_iid" :
                           row["p1_spw_iid"], "p2_spw_iid" : row["p2_spw_iid"],
                           "p1_average_spw" : p1_average_iid, "p2_average_spw" :
                           p2_average_iid}

            results.append(cur_results)

    results = pd.DataFrame(results)

    spw_diff_model = results["p1_spw_iid"] - results["p2_spw_iid"]
    spw_diff_av = results["p1_average_spw"] - results["p2_average_spw"]

    plt.scatter(results["p1_average_spw"] - results["p2_average_spw"],
                results["p1_spw_iid"] - results["p2_spw_iid"])
    plt.show()

    test = np.polyfit(spw_diff_av, spw_diff_model, 1)

    print(test)

    print(np.average(results["iid_games"] - results["games"]))
    print(np.average(results["dynamic_games"] - results["games"]))

def explore_input():

    df = pd.read_csv("../wta_points_predicted_player_no_tournament_name.csv")

    no_events = (df["tiebreak"] == False) & (df["breakpoint"] == False) & \
                (df["before_breakpoint"] == False) 

    no_advantage = df[(no_events) & (df["set_up"] == 0) & (df["set_down"] == 0)]
    set_down = df[(no_events) & (df["set_up"] == 0) & (df["set_down"] == 1)]

    difference = no_advantage["p_noniid"].values - set_down["p_noniid"].values

    print(round(np.average(no_advantage["p_noniid"].values), 3),
          round(np.average(set_down["p_noniid"].values), 3),
          round(np.average(difference), 3), round(np.std(difference),3))

    plt.scatter(no_advantage["p_noniid"].values, set_down["p_noniid"].values)
    plt.show()

    exit()

t_type = "atp"

if (t_type == "wta"):
    df = pd.read_csv('../iid_vs_non_iid_wta_players_1e4trials.csv')
    df_results = pd.read_csv('../wta_points_predicted_player.csv')
elif (t_type == "atp"):
    df = pd.read_csv('../iid_vs_non_iid_atp_players_1e4trials.csv')
    df_results = pd.read_csv('../atp_points_predicted_player.csv')

df = add_ranking(df, df_results)

# add_atp_data("matches_2014_atp.csv", df)

class PredictionSet:
    def __init__(self, predictions_file, tournament, year, t_type):
        self.predictions_file = predictions_file
        self.tournament = tournament
        self.year = year
        self.t_type = t_type

predictions = [PredictionSet("../ausopen_predictions_iid.csv",
                             "australian open",
                             2015,
                             "atp"),
               PredictionSet("../usopen_predictions_iid.csv",
                             "us open",
                             2014,
                             "atp"),
               PredictionSet("../wimbledon_predictions_iid.csv",
                             "wimbledon",
                             2014,
                             "atp")]

predictions_file = "../ausopen_predictions_iid.csv"
tournament = "australian open"
year = 2015
t_type = "atp"

for p_set in predictions:
    check_predictions(p_set.predictions_file,
                      p_set.tournament,
                      p_set.year,
                      p_set.t_type)

# plot_by_ranking(df)
# plot_deviation_histogram(df)
# plot_average_offset(df)

print(df.columns)
