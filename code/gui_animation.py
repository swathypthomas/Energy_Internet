import redis
import networkx as nx
import matplotlib.pyplot as plt


reply = redis.Redis(host='127.0.0.1', port=6379)
selfNode = reply.get('ID')
G = nx.DiGraph(directed=True)
f = 1;
while f == 1:
    print("HI0")
    reply.blpop(str('CONNECTION_UPDATE'), timeout=int(0))
    print("HI1")
    plt.clf()
    peerIp = []
    peerNodes = []
    G.clear()
    peers = reply.hgetall('PEER:IP:PORT')
    for i in peers.keys():
        peerIp.append(i.decode('utf-8'))
    for i in peerIp:
        reply = redis.Redis(host=str(i), port=6379)
        peerId = reply.get('ID') 
        peerNodes.append(peerId.decode('utf-8'))
    for i in peerNodes:
        G.add_edge(selfNode, i, weight=0.6)
    if nx.is_empty(G):
        G.add_node(selfNode)
    print(peerNodes)
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
    plt.savefig("graph.png", format="PNG", dpi = 33, facecolor='w', edgecolor='w', transparant = True, bbox_inches="tight")