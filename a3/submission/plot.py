import os
import subprocess
from subprocess import PIPE,Popen, TimeoutExpired
import matplotlib.pyplot as plt
import time

arr1=[0,0,0]
arr2=[0,0,0]
arr3=[0,0,0]
arr4=[0,0,0]
arr5=[0,0,0]
n = [10,50,100]

os.system("g++ scheduling.cpp -o schedule")

for i in range(30):	
	p = subprocess.Popen(['./schedule'],stdin=PIPE,stdout=PIPE)
	# print p.stdout.readline()
	try:
		if(i//10==0):
			stdout, stderr = p.communicate(input=b'10\n0.5\n', timeout=1)
		if(i//10==1):
			stdout, stderr = p.communicate(input=b'50\n0.5\n', timeout=1)
		if(i//10==2):
			stdout, stderr = p.communicate(input=b'100\n0.5\n', timeout=1)
		words=stdout.split();
		print(words[0], words[1], words[2], words[3], words[4])	
		arr1[i//10] = arr1[i//10]+float(words[0].decode('UTF-8'))
		arr2[i//10] = arr2[i//10]+float(words[1].decode('UTF-8'))
		arr3[i//10] = arr3[i//10]+float(words[2].decode('UTF-8'))
		arr4[i//10] = arr4[i//10]+float(words[3].decode('UTF-8'))
		arr5[i//10] = arr5[i//10]+float(words[4].decode('UTF-8'))
	except TimeoutExpired:
		p.kill()
		i=i-1	


arr1 = [i/10 for i in arr1]
arr2 = [i/10 for i in arr2]
arr3 = [i/10 for i in arr3]
arr4 = [i/10 for i in arr4]
arr5 = [i/10 for i in arr5]

colors = ["green", "blue", "red", "magenta", "black"]
labels = ["fcfs", "npsjf", "psjf", "roundrobin", "hrn"]

y = n
for i in range(5):
	if(i==0):
		x = arr1
	if(i==1):
		x = arr2
	if(i==2):
		x = arr3
	if(i==3):
		x = arr4
	if(i==4):
		x = arr5
	plt.plot(y, x, c=colors[i], alpha=0.5, marker='x', label= labels[i])


plt.legend(loc='upper left')
plt.title('Average turnaround time vs N')
plt.xlabel('N')
plt.ylabel('Average turnaround time')
plt.xticks([10,50,100])
plt.show()