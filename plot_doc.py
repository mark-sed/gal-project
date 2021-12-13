"""
"""

__author__ = "Jiří Křištof"
__contact__ = "xkrist22@stud.fit.vutbr.cz"
__date__ = "11-12-2021"


from evaluate import Evaluate


if __name__ == "__main__":
    e = Evaluate("in.csv", "out.csv")
    e.plot_line_chart(0, 71, "node_num", "time", "sparse_test.png")
    e.plot_line_chart(72, 143, "node_num", "time", "dense_test.png")
    e.plot_line_chart(144, 215, "constraints_num", "time", "constraints_test.png")
    e.plot_line_chart(216, 287, "edge_num", "time", "edge_num_test.png")
    e.plot_line_chart(288, 359, "colors", "time", "colors_sparse_test.png")
    e.plot_line_chart(360, 431, "colors", "time", "colors_dense_test.png")
    e.plot_line_chart(432, 503, "constraints_num", "time", "collision_test.png")