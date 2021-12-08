from MaxHeap import MaxHeap

def Dijkstras_with_H(G,s,t,n):
    status = ['unseen']*n
    status[s] = 'in-tree'
    F = MaxHeap()
    dad = [-1]*n
    bw = [0]*n
    for w in G.edges[s]:
        status[w[0]] = 'fringe'
        dad[w[0]] = s
        bw[w[0]] = w[1]
        F.insert(w[0],w[1])
    while 1:
        v = F.maximum()
        if v is None:
            break
        status[v] = "in-tree"
        for w in G.edges[v]:
            if status[w[0]] == 'unseen':
                status[w[0]] = 'fringe'
                bw[w[0]] = min(bw[v], w[1])
                dad[w[0]] = v
                F.insert(w[0],w[1])
            elif status[w[0]] == 'fringe' and bw[w[0]] < min(bw[v], w[1]):
                F.delete(w[0])
                bw[w[0]] = min(bw[v], w[1])
                F.insert(w[0],w[1])
                dad[w[0]] = v
    if status[t] != 'in-tree':
        print('no-path')
        return
    else:
        x = t
        b = 1000000
        while x!=s:
            #if bw[x] < b:
            #    b = bw[x]
            print(str(x),end= " ")
            x = dad[x]
        print(str(s))
        #print("BW: " + str(b))
        return


