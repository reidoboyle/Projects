class MaxHeap():
    def __init__(self):
        self.size = 0
        self.max = 5000
        self.H = [-1 for x in range(0,5000)]    # holds the vertex values
        self.D = [-1 for x in range(0,5000)]    # holds the bandwidth of vertex
        self.P = [-1 for x in range(0,5000)]    # holds the position of a vertex in the heap
    def swap(self,a,b):
        temp = self.H[a]
        self.H[a] = self.H[b]
        self.H[b] = temp

        self.P[self.H[a]] = a
        self.P[self.H[b]] = b
    def heapify(self,i):
        left = 2*i
        right = 2*i + 1
        parent = i//2
        largest = None
        if not (i >= self.size//2 and i <= self.size) and i < self.size:

            try:
                if self.D[self.H[i]] < self.D[self.H[left]] or self.D[self.H[i]] < self.D[self.H[right]]:
                    if self.D[self.H[left]] > self.D[self.H[right]]:
                        self.swap(i,left)
                        self.heapify(left)
                    else:
                        self.swap(i,right)
                        self.heapify(right)
            except:
                print(self.size,i)
    def insert(self,x,bw):
        if self.size > self.max:
            return
        self.size += 1
        self.H[self.size] = x
        self.P[self.H[self.size]] = self.size
        self.D[x] = bw
        if self.size == 1:
            return
        idx = self.size
        while self.D[self.H[idx]] > self.D[self.H[idx//2]] and idx > 1:
            self.swap(idx,idx//2)
            idx = idx//2

    def maximum(self):
        m = self.H[1]
        self.H[1] = self.H[self.size]
        self.H[self.size] = -1
        self.size -= 1
        self.heapify(1)
        if self.size < 0:
            return None
        else:
            return m
    def delete(self,v):
        index = self.P[v]

        old = self.D[self.H[index]]
        self.H[index] = self.H[self.size]
        new = self.D[self.H[index]]
        self.H[self.size] = -1
        self.P[v] = -1
        self.P[self.H[index]] = index
        self.size -= 1

        # heapify
        if new < old:
            self.heapify(index)
        else:
            idx = index
            while self.D[self.H[idx]] > self.D[self.H[idx // 2]] and idx > 1:
                self.swap(idx, idx // 2)
                idx = idx // 2