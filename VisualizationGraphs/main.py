import matplotlib.pyplot as plt
import itertools

def formation_complete_graph_edges(points):
    complete_graph_edges = list(itertools.combinations(range(len(points)), 2))
    return complete_graph_edges

def visualize_graph_and_tree(points, graph_edges, tree_edges, clusters):

    # dots
    x_values = [point[0] for point in points]
    y_values = [point[1] for point in points]
    plt.scatter(x_values, y_values, color='blue', label='Points')

    # start graph
    for edge in graph_edges:
        x1, y1 = points[edge[0]]
        x2, y2 = points[edge[1]]
        plt.plot([x1, x2], [y1, y2], color='gray', linestyle='-', linewidth=1)

    # Tree
    for edge in tree_edges:
        x1, y1 = points[edge[0]]
        x2, y2 = points[edge[1]]
        plt.plot([x1, x2], [y1, y2], color='red', linestyle='-', linewidth=2)

    # Annotate points with their index
    for idx, (x, y) in enumerate(points):
        plt.annotate(str(idx), (x, y), textcoords="offset points", xytext=(-3,5), ha='center', fontsize=10, color='green')

    # Clusters
    for edge in clusters:
        x1, y1 = points[edge[0]]
        x2, y2 = points[edge[1]]
        plt.plot([x1, x2], [y1, y2], color='yellow', linestyle='-', linewidth=2)

    plt.xlabel('X')
    plt.ylabel('Y')
    plt.title('Graph and Minimum Spanning Tree Visualization')
    plt.grid(True)
    plt.axis('equal')  # Одинаковый масштаб осей X и Y
    plt.show()

def read_from_file(points_file, tree_file, clusters_file):
    points = []
    tree_edges = []
    clusters = []
    with open(points_file, 'r') as f:
        for line in f:
            el = list(map(float, line.strip().split(' ')))
            points.append(el)
    with open(tree_file, 'r') as f:
        for line in f:
            el = list(map(int, line.strip().split(' ')))
            tree_edges.append(el)
    with open(clusters_file, 'r') as f:
        for line in f:
            el = list(map(int, line.strip().split(' ')))
            clusters.append(el)
    return points, tree_edges, clusters

points, tree_edges, clusters = read_from_file("dots.txt", "tree.txt", "clusters.txt")
graph_edges = formation_complete_graph_edges(points)

visualize_graph_and_tree(points, graph_edges, tree_edges, clusters)