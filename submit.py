import operator

mydict={}

for line in open("unigram.txt",'r'):
	temp = line.split()
	#mydict[temp[0]] = int(temp[1])
	if mydict.has_key(temp[0]):
		continue
	else:
		mydict[temp[0]] = int(temp[1])



#print mydict.keys()
#print mydict[u'\u7684']

sorted_x = sorted(mydict.iteritems(), key=operator.itemgetter(1), reverse=True)


f=open('unigram_sorted.txt', 'w')
for i in sorted_x:
    f.write(i[0]+' '+str(i[1])+'\n')
f.close()

mydict2={}
for line in open("bigram.txt"):
	temp = line.split(" ")
	value = temp[0]+' '+temp[1];
	if mydict2.has_key(value):
		continue
	else:
		mydict2[value] = int(temp[2])

sorted_y = sorted(mydict2.iteritems(), key=operator.itemgetter(1), reverse=True)

f2=open('bigram_sorted.txt', 'w')
for i in sorted_y:
    f2.write(i[0]+' '+str(i[1])+'\n')
f2.close()