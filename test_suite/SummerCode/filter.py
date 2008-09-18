import re,gzip,zlib,sys

filename = sys.argv[1]

while True:
	gf = gzip.open(filename)
	filecontents = gf.read()
	if len(filecontents)==0:
		print "done" 
		exit()
	regex = filecontents[0:7]
	r = re.compile("("+regex +".*\n.*\n.*\n)")
	outfile = regex

	gf = gzip.open(filename)

	#print r.sub("",gf.read())
	wf = gzip.open(filename[0:-2]+outfile+".gz","w")
	splits = r.split(gf.read())
	for i in splits[1::2]:
		wf.write(i)
	#wf.write(r.split(gf.read())[1::2])
	gf.close()
	wf = gzip.open(filename, "w")
	for i in splits[2::2]:
		wf.write(i)
	wf.close()
