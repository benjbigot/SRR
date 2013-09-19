#! /usr/bin/python
# -*- coding: utf-8 -*-
'''
Created on 6 dec. 2011
@author: benjaminbigot
'''
#===================================================================#
def _getParam(fileCfg) :
	for line in fileCfg :
			if "param:" in line.lower() :
				return line
#===================================================================#	
def _getRole(fileCfg) :
	for line in fileCfg :
			if "role:" in line.lower() : 
				return line
#===================================================================#
def loadConfig(cfgFile) :
	try :
		fileCfg = open(cfgFile, "r")  
	except IOError:
		print("Error: can\'t find file or read data file named : ", fileCfg)
		parser.print_help()
	else:
		param = _getParam(fileCfg)
		role = _getRole(fileCfg)
		fileCfg.close()
		#print(param, role)
	return param.rstrip().split()[1:] , role.rstrip().split()[1:]
#====================================================================#
def _getRoleType(line):
	return line.split('speaker="')[1].split('.')[1].split('"')[0]
#====================================================================#
def _getRoleId(spk, roles):
	for i in roles :
		if i.split('=')[0] == spk :
			return i.split('=')[1]
	return -1
#====================================================================#
def _getOrderedParams(line, param):
	tempLine= []
	for i in range(0, len(param)):
		for j in line.split() :
			#print(param[i].split('=')[0] , j.split('=')[0] )
			if j.split('=')[0] == param[i].split('=')[0] :
				#print(j.split('=')[0], param[i].split('=')[0] )
				tempLine.append( str(i+1) + ':' + str(float(j.split('"')[1])))
	#print (tempLine)
	return ' '.join(tempLine)
#====================================================================#
def editOut(roleP, parameter, role) :
	''' roleP include the speaker role and its parameters '''
	out = []
	for line in roleP :
		#print line
		currentLine=[]
		roleType = _getRoleId(_getRoleType(line) , role)
		#print(_getRoleType(line))
		#print(roleType)
		if roleType >= 0 :
			currentLine.append(roleType)
			#print(_getOrderedParams(line, parameter))
			out.append(' '.join( [roleType ,_getOrderedParams(line, parameter)]))
	return out
#====================================================================#
def writeOut(matrice, fichier) :
	for line in matrice :
		fichier.write(line+"\n")
	return 1
#====================================================================#
def writingParameters(roleP, configFile, outputSVM):
	parameters , role = loadConfig(configFile)
	#print(parameters, role, roleP)
	outMat = editOut( roleP, parameters, role)
	#print(outMat)
	try :       
		fileOut = open(outputSVM, "w")  
	except IOError:
		print("Error: can\'t find file or read data file named : ", outputSVM)
		parser.print_help()
	else:
		writeOut(outMat, fileOut)
		fileOut.close()
	#print(outMat)

#====================================================================	
if __name__=='__main__' :
	parser = argparse.ArgumentParser(description='Main script for speaker role recognition parameter extraction.')
	parser.add_argument('-i', '--input' , dest='inputFile', help='Speaker activity matrix file (mdtm)', required=True )
	parser.add_argument('-l', '--inputList', dest='inputList', help='speaker id list', required=True)
	parser.add_argument('-w', '--wavFile', dest='wavFile', help='wav file', required=True)	
#====================================================================	
