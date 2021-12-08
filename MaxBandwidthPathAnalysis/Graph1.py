import random

class G1():
    def __init__(self):
        print("Initializing G1")
        self.avg_vtx = 6
        self.n = 5000
        self.edges = self.create_edges()
        self.single_edges = []
        self.degree = [0] * self.n
        self.available = [x for x in range(5000)]
        self.connect_graph()
        self.add_random_edges()

    def create_edges(self):
        print('creating initial edges')
        edges = []
        for i in range(self.n):
            e = []
            edges.append(e)
        return edges

    def connect_graph(self):
        print('connecting graph')
        vertices = [x for x in range(0, self.n)]
        random.shuffle(vertices)
        for i in range(self.n):
            j = i + 1
            if j == self.n:
                j = 0
            v1 = vertices[i]
            v2 = vertices[j]
            wgt = random.randint(10000, 100000)
            self.edges[v1].append([v2,wgt])
            self.edges[v2].append([v1,wgt])
            self.degree[v1] += 1
            self.degree[v2] += 1
            self.single_edges.append([[v1,v2],wgt])

    def add_random_edges(self):
        print("Adding random edges")

        while sum(self.degree)/5000.0 < 6.0:
            i = random.randint(0,4999)
            j = random.randint(0,4999)
            while i == j:
                j = random.randint(0,4999)
            wgt = random.randint(10000, 100000)
            self.edges[i].append([j,wgt])
            self.edges[j].append([i,wgt])
            self.degree[i] += 1
            self.degree[j] += 1
            self.single_edges.append([[i, j], wgt])
