import redis
import networkx as nx
import matplotlib.pyplot as plt

G = nx.DiGraph(directed=True)
G.add_edge('ER3', 'ER1', weight=0.6)
pos = nx.spring_layout(G)  # positions for all nodes
options = {
    'node_color': 'darkblue',
    'node_size': 2000,
    'width': 3,
    'arrowstyle': '-|>',
    'arrowsize': 25,
    'font_size': 20,
    'edge_color': 'b',
    'font_color': 'w',
    'linewidths': 0
}
nx.draw_networkx(G, arrows=True, **options)
plt.axis('off')
plt.savefig("graphER3.png", format="PNG", dpi = 33, facecolor='w', edgecolor='w', transparant = True, bbox_inches="tight")