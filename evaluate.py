"""
"""

__author__ = "Jiří Křištof"
__contact__ = "xkrist22@stud.fit.vutbr.cz"
__date__ = "11-12-2021"


import argparse
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np


class Evaluate:
    def __init__(self, input_file: str = "in.csv", output_file: str = "out.csv") -> None:
        """
        Constructor of evaluation unit

        Args:
            input_file (str): name of input benchmark file, defaultly "in.csv"
            output_file (str): name of output from gal.out benchmark mode, defaultly "out.csv"
        """

        self.__labels = {
            "time": "duration of algorithm [ms]",
            "node_num": "number of vertices",
            "edge_num": "number of edges",
            "constraints_num": "total number of constraints",
            "colors": "colors avaiable",
            "success": "success rate"
        }
        
        self.in_cols = [
            "id",
            "algorithm",
            "graph_file",
            "constraints_file",
            "colors",
            "population",
            "repetition"
        ]
        self.out_cols = [
            "id",
            "time",
            "success",
            "node_num",
            "edge_num",
            "constraints_num"
        ]

        # load datasets from given files
        self.__in_df = pd.read_csv(input_file, names=self.in_cols)
        self.__out_df = pd.read_csv(output_file, names=self.out_cols)

        # parse datasets into one dataset
        self.__df = self.__out_df.groupby("id").agg({
            "time": "median",
            "success": "mean",
            "node_num": "first",
            "edge_num": "first",
            "constraints_num": "first"
        })

        # join data from benchmarking with data from benchmark settings 
        # and remove unnecessary data
        self.__df = self.__df.join(self.__in_df.set_index("id"), on='id')
        self.__df = self.__df.drop(["graph_file", "constraints_file"], axis=1)


    def plot_line_chart(self, start_id: int, stop_id: int, x_axe_var: str, y_axe_var: str, output_filename: str, title: str = None) -> None:
        """
        Method plots graph using values describing values to be used and variables to be plotted

        Args:
            start_id: number of id from which data relevant for plotting will be taken
            stop_id: number of id to which data relevant for plotting will be taken
            x_axe_var: variable to be used at x-axe of graph
            y_axe_var: variable to be used at y-axe of graph

        Note:
            If having ids from 0 to 20 and start_id = 5 and stop_id = 10, then only
                data with id 5, 6, 7, 8, 9 and 10 will be used
            Axes variables must be one of column names of dataframe
        """
        
        local_df = self.__df.loc[(self.__df.index >= start_id) & (self.__df.index <= stop_id)]

        plt.plot(local_df.loc[local_df["algorithm"] == "g", x_axe_var], local_df.loc[local_df["algorithm"] == "g", y_axe_var], label="greedy")
        plt.plot(local_df.loc[local_df["algorithm"] == "e", x_axe_var], local_df.loc[local_df["algorithm"] == "e", y_axe_var], label="evolution")
        plt.plot(local_df.loc[local_df["algorithm"] == "h", x_axe_var], local_df.loc[local_df["algorithm"] == "h", y_axe_var], label="heuristic")
        plt.legend()
        plt.xlabel(self.__labels[x_axe_var])
        plt.ylabel(self.__labels[y_axe_var])
        if title:
            plt.title(title)
        plt.savefig(output_filename)
        plt.clf()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Script for generating statistics from benchmarking")
    parser.add_argument(
        "--benchmark-input", "-bi",
        action="store",
        dest="benchmark_input",
        default="in.csv",
        help="Define name of file for benchmarking"
    )
    parser.add_argument(
        "--benchmark-output", "-bo",
        action="store",
        dest="benchmark_output",
        default="out.csv",
        help="Define name of file for benchmarking"
    )
    parser.add_argument(
        "--start-index", "-si",
        action="store",
        dest="start_index",
        required=True,
        help="Define starting index from which data will be taken"
    )
    parser.add_argument(
        "--ending-index", "-ei",
        action="store",
        dest="end_index",
        required=True,
        help="Define ending index from which data will be taken"
    )
    parser.add_argument(
        "--x-axis-var", "-x",
        action="store",
        dest="x_axe",
        required=True,
        help="Define which variable will be at x axe"
    )
    parser.add_argument(
        "--y-axis-var", "-y",
        action="store",
        dest="y_axe",
        required=True,
        help="Define which variable will be at y axe"
    )
    parser.add_argument(
        "--output-file", "-of",
        action="store",
        dest="output_file",
        default="out.png",
        help="Define name of file into which graph will be saved, defaultly out.png"
    )

    arguments = parser.parse_args()

    e = Evaluate(arguments.benchmark_input, arguments.benchmark_output)
    e.plot_line_chart(int(arguments.start_index), int(arguments.end_index), arguments.x_axe, arguments.y_axe, arguments.output_file)