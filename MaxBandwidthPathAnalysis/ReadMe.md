
Project where I ran different Max Bandwidth Path algorithms on different graph structures and reported my findings

MBP Algorithms tested (m = number ofdges, n=number of vertices)
- Dijkstras (original with fringes held in a list) -> O(n^2)
- Dijkstras (holding fringes in a max heap) -> (mlogn)
- Kruskals (edges are sorted by heapSort) -> (mlogn)

Graph Structures Algorithms where tested on
- G1 - Average vertex degree is 6 (5000 vertices)
- G2 - Each vertex is adjacent to apx. 20% of the other vertices (5000 vertices)
- Edges are randomly assigned positive weights

main.py runs the algorithms and collects the data
Findings and results are observed in analysis.py and discussed in the project report PDF
