class Kruskals:
    def __init__(self):
        self.n = 5000
        self.h = [0] * 5000
        self.p = [0] * 5000
        # sort the edges in decreasing order
        self.color = ['white' for x in range(5000)]
        self.dad = [0 for b in range(5000)]
        self.edges = []

    def Union(self,r1,r2):
        if self.h[r1] > self.h[r2]:
            self.p[r2] = r1
        elif self.h[r2] > self.h[r1]:
            self.p[r1] = r2
        else:   # h[r1] == h[r2]
            self.p[r2] = r1
            self.h[r1] = self.h[r1] + 1

    def Makeset(self,v):
        self.p[v] = -1
        self.h[v] = 0

    def Find(self,v):
        w = v
        while self.p[w] != -1:
            w = self.p[w]
        return w
    def max_heapify(self,edges):
        size = 0
        heap = [-1 for i in range(0,len(edges)+1)]
        for e in edges:
            size += 1
            heap[size] = e

            if size == 1:
                continue
            idx = size

            while idx > 1:
                if heap[idx][1] > heap[idx//2][1]:
                    temp = heap[idx]
                    heap[idx] = heap[idx//2]
                    heap[idx//2] = temp
                    idx = idx//2
                else:
                    break
        return heap

    def heapsort(self):
        for i in range(len(self.edges)-1,1,-1):
            self.edges[0],self.edges[i] = self.edges[i],self.edges[0]
            self.downheap(i,1)
    def downheap(self,size,i):
        left = 2 * i
        right = 2 * i + 1
        largest = 0
        if left < size and self.edges[left][1] > self.edges[i][1]:
            largest = left
        else:
            largest = i
        if right < size and self.edges[right][1] > self.edges[largest][1]:
            largest = right
        if largest != i:
            self.edges[largest],self.edges[i] = self.edges[i],self.edges[largest]
            self.downheap(size,largest)

    def DFS(self,graph,v):
        self.DFS_recurse(graph,v)
    def DFS_recurse(self,graph,v):
        self.color[v] = 'grey'
        for w in graph[v]:
            if self.color[w] == 'white':
                self.dad[w] = v
                self.DFS_recurse(graph,w)
        self.color[v] = 'black'
    def Main(self,G,s,t):
        n = 5000
        self.edges = self.max_heapify(G.single_edges)
        self.heapsort()
        self.edges = self.edges[1:]

        """        z = len(edges)-1
        while z > 1:
            sorted_edges.append(edges[1])
            edges[1] = edges[z]
            edges = edges[:len(edges)-1]
            z -= 1
            edges = self.downheap(edges,z,1)
        sorted_edges.append(edges[z])"""

        m = len(self.edges)
        T = []
        for v in range(5000):
            self.Makeset(v)

        for k in range(1,m-1):
            e_k = self.edges[k]
            u_k = e_k[0][0]
            v_k = e_k[0][1]
            r1 = self.Find(u_k)
            r2 = self.Find(v_k)
            if r1 != r2:
                T.append(e_k)
                self.Union(r1,r2)

        # recreate a list of edges that we can do a DFS on
        graph_edges = [[] for x in range(5000)]
        for e in T:
            graph_edges[e[0][0]].append(e[0][1])
            graph_edges[e[0][1]].append(e[0][0])

        self.DFS(graph_edges,s)
        #bw = 100000
        if self.color[t] == "white":
            print('t not reachable from s')
        else:
            x = t
            while x != s:
                #for w in G.edges[x]:
                #    if w[0] == dad[x]:
                #        if w[1] < bw:
                #            bw = w[1]
                print(str(x),end=" ")
                x = self.dad[x]
            print(str(s))
            #print("BW: " + str(bw))
            return


def Kruskals_Main(G,s,t):
    K = Kruskals()
    K.Main(G,s,t)