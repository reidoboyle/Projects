import matplotlib.pyplot as plt

kruskal_g1_times = [0.03774249999999313, 0.039481899999998404, 0.036567099999984976, 0.037081299999982775, 0.034761099999997214, 0.05063790000002655, 0.03389559999999392, 0.03191250000003265, 0.03519260000001623, 0.033577699999966626, 0.03156380000001491, 0.03162660000003825, 0.03157570000001897, 0.03476630000000114, 0.03224790000001576, 0.03374939999991966, 0.03352440000003298, 0.03287020000004759, 0.03367279999997663, 0.0491329000000178, 0.038505100000065795, 0.03360499999996591, 0.032352699999933066, 0.03293440000004466, 0.033372200000030716]
kruskal_g2_times = [5.887645200000009, 5.905214700000002, 5.945514599999996, 5.321925399999998, 5.0491097000000025, 5.730899000000022, 5.353025899999977, 5.260041099999967, 5.228704899999968, 5.17761820000004, 5.204585000000009, 5.166817700000024, 5.125883999999985, 5.535106799999994, 5.365738999999962, 5.319114300000024, 6.13763449999999, 5.227273999999966, 5.321081700000036, 6.847849699999983, 5.120928499999991, 5.31401900000003, 6.191666499999997, 6.662831900000015, 6.17223690000003]
djk_nh_g1_times = [1.5382180000000005, 1.4587410999999975, 1.528568699999994, 1.680769099999992, 1.3401331000000027, 1.2107151999999815, 1.3734598000000346, 1.2745270000000346, 1.9566975000000184, 1.2999443000000497, 1.2857184999999731, 1.2858866999999918, 1.5476894000000243, 1.3808558999999718, 1.2770706000000018, 1.420437100000072, 1.278403600000047, 1.3745594000000665, 1.2824458999999706, 2.445828499999948, 1.3203306000000339, 1.248338600000011, 1.3583083999999417, 1.4519079000000374, 1.977176600000007]
djk_nh_g2_times = [3.6799536000000046, 3.638232000000002, 3.549003699999986, 3.0897860999999978, 3.0570965, 3.483548400000018, 3.4456174000000033, 3.210186999999962, 3.3767778000000135, 3.164799999999957, 3.0156862000000046, 3.144096100000013, 6.660009699999989, 3.124180000000024, 3.5742985999999632, 3.2079086999999618, 3.1559495000000197, 3.2893113000000085, 4.178126999999904, 8.94975039999997, 3.0947714000000133, 3.1761030999999775, 3.4019119999999248, 3.5963669999999865, 3.4526462999999694]
djk_g1_times = [0.12994170000000338, 0.1004136000000102, 0.09916259999999966, 0.09475219999998785, 0.09713020000000938, 0.08252769999995735, 0.09699490000002697, 0.09091730000000098, 0.11004830000001675, 0.09720909999998639, 0.09388480000001209, 0.09600839999995969, 0.09684859999998707, 0.12228350000003729, 0.1062430000000063, 0.19227409999996325, 0.09416959999998653, 0.1025025999999798, 0.09475380000003497, 0.09482649999995374, 0.089463300000034, 0.08514440000010381, 0.2540149000000156, 0.09301090000008116, 0.11347589999991214]
djk_g2_times = [2.2210409999999996, 2.1825984000000034, 2.253157499999986, 1.8252178000000185, 1.9178869000000134, 1.8098143999999934, 2.051365099999998, 1.802892600000007, 2.4307231999999885, 1.8277385000000095, 1.7763965000000326, 1.7908810999999787, 1.8037529000000063, 1.9279943999999887, 2.275146300000017, 4.378892099999916, 1.8132806999999502, 2.495225399999981, 1.7661123999999973, 2.0519577000000027, 1.88560700000005, 1.8071916000000101, 3.6385328000000072, 2.3541493000000173, 2.798548500000038]
s_d = [[405, 3957], [2060, 1426], [571, 3445], [4580, 3748], [4828, 4262], [621, 1628], [2772, 3938], [2776, 60], [3571, 764], [1341, 491], [2281, 728], [3136, 3175], [4681, 1265], [2857, 2235], [2847, 2302], [319, 2886], [3612, 487], [4741, 761], [3932, 4816], [1865, 4363], [4273, 13], [1765, 4466], [117, 3402], [829, 2867], [1580, 1036]]
iteration = [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25]

avg_g1_time = (sum(kruskal_g1_times)/25.0 + sum(djk_nh_g1_times)/25.0 + sum(djk_g1_times)/25.0)/3.0
avg_g2_time = (sum(kruskal_g2_times)/25.0 + sum(djk_nh_g2_times)/25.0 + sum(djk_g2_times)/25.0)/3.0

plt.bar([1,2],[avg_g1_time,avg_g2_time],color=['red','blue'],tick_label=["G1","G2"])
plt.title("Average Running time of all algorithms for G1 and G2 Graphs")
plt.ylabel("Seconds")
plt.text(0.75,avg_g1_time + 0.1,str(avg_g1_time))
plt.text(1.75,avg_g2_time,str(avg_g2_time))
plt.show()

plt.bar([1,2,3],[sum(kruskal_g1_times)/25,sum(djk_nh_g1_times)/25,sum(djk_g1_times)/25],color=['red','blue','purple'],tick_label=['Kruskals',"Dijkstra's without Heap","Dijkstra's with Heap"])
plt.title("Average Running Time of All 3 Algorithms on G1")
plt.ylabel("Seconds")
plt.text(.5,sum(kruskal_g1_times)/25,str(sum(kruskal_g1_times)/25))
plt.text(1.5,sum(djk_nh_g1_times)/25,str(sum(djk_nh_g1_times)/25))
plt.text(2.5,sum(djk_g1_times)/25,str(sum(djk_g1_times)/25))
plt.show()

plt.bar([1,2,3],[sum(kruskal_g2_times)/25,sum(djk_nh_g2_times)/25,sum(djk_g2_times)/25],color=['red','blue','purple'],tick_label=['Kruskals',"Dijkstra's without Heap","Dijkstra's with Heap"])
plt.title("Average Running Time of All 3 Algorithms on G2")
plt.ylabel("Seconds")
plt.text(.5,sum(kruskal_g2_times)/25,str(sum(kruskal_g2_times)/25))
plt.text(1.5,sum(djk_nh_g2_times)/25,str(sum(djk_nh_g2_times)/25))
plt.text(2.5,sum(djk_g2_times)/25,str(sum(djk_g2_times)/25))
plt.show()

plt.plot(iteration,kruskal_g1_times,color='blue',label='Kruskal Times on G1')
plt.plot(iteration,kruskal_g2_times,color='skyblue',label='Kruskal Times on G2')
plt.plot(iteration,djk_g1_times,color='red',label='Dijkstras with heap Times on G1')
plt.plot(iteration,djk_g2_times,color='orange',label='Dijkstras with heap Times on G2')
plt.plot(iteration,djk_nh_g1_times,color='purple',label='Dijkstras w/out heap Times on G1')
plt.plot(iteration,djk_nh_g2_times,color='pink',label='Dijkstras w/out heap Times on G2')
plt.ylabel("Seconds")
plt.xlabel("Iteration")
plt.title("All algorithm times on both G1 and G2 ")
plt.legend()
plt.show()
