import random

class G2():
    def __init__(self):
        print('Initializing G2')
        self.n = 5000
        self.adjacency_pct = 0.20*self.n
        self.edges = self.create_edges()
        self.adj_pct = [0]*self.n
        self.single_edges = []
        #self.available = [x for x in range(5000)]
        self.connect_graph()
        self.create_adjacencies()

    def create_edges(self):
        print('creating initial edges')
        edges = []
        for i in range(self.n):
            e = []
            edges.append(e)
        return edges
    def connect_graph(self):
        print('connecting graph')
        vertices = [x for x in range(0,self.n)]
        random.shuffle(vertices)
        for i in range(self.n):
            j = i+1
            if j == self.n:
                j = 0
            v1 = vertices[i]
            v2 = vertices[j]
            wgt = random.randint(10000,100000)
            self.edges[v1].append([v2,wgt])
            self.edges[v2].append([v1,wgt])
            self.single_edges.append([[v1, v2], wgt])

        self.calculate_adjacency()
    def calculate_adjacency(self):
        print('calculating adjacency percentages')
        for i in range(self.n):
            self.adj_pct[i] = len(self.edges[i])

    def create_adjacencies(self):
        print('creating adjacency percentages of 20%')
        #print(self.total_adj_pct())
        for i in range(self.n):
            while self.adj_pct[i] <= self.adjacency_pct-250:
                #random.shuffle(self.available)
                idx = random.randint(0,self.n-1)
                duplicate = False

                for edge in self.edges[i]:
                    if edge[0] == idx:
                        duplicate = True

                if not duplicate:
                    wgt = random.randint(10000,100000)
                    self.edges[i].append([idx, wgt])
                    self.edges[idx].append([i, wgt])
                    self.adj_pct[i] += 1
                    self.adj_pct[idx] += 1
                    self.single_edges.append([[i, idx], wgt])

        while sum(self.adj_pct)/5000 < 1000:
            x = random.randint(0,4999)
            y = random.randint(0,4999)
            wgt = random.randint(10000, 100000)
            if x != y:
                self.edges[x].append([y, wgt])
                self.edges[y].append([x, wgt])
                self.adj_pct[x] += 1
                self.adj_pct[y] += 1
                self.single_edges.append([[x, y], wgt])
