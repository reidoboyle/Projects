
def Dijkstras(G,s,t,n):
    status = ['unseen']*n
    status[s] = 'in-tree'
    dad = [-1]*n
    bw = [0]*n
    for w in G.edges[s]:
        status[w[0]] = 'fringe'
        dad[w[0]] = s
        bw[w[0]] = w[1]
    while 1:
        fringe = None
        l_bw = 0
        v = -1
        for i in range(0,n):
            if status[i] == 'fringe' and bw[i] > l_bw:
                v = i
                l_bw = bw[i]
                fringe = True
        if fringe is None:
            break
        else:
            status[v] = 'in-tree'
            for w in G.edges[v]:
                if status[w[0]] == 'unseen':
                    status[w[0]] = 'fringe'
                    bw[w[0]] = min(bw[v], w[1])
                    dad[w[0]] = v
                elif status[w[0]] == 'fringe' and bw[w[0]] < min(bw[v], w[1]):
                    bw[w[0]] = min(bw[v], w[1])
                    dad[w[0]] = v

    if status[t] != 'in-tree':
        print('no-path')
        return
    else:
        x = t
        b = 100000
        while x!=s:
            #if bw[x] < b:
            #    b = bw[x]
            print(str(x),end=" ")
            x = dad[x]
        print(str(s))
        #print("BW: " + str(b))
        return


