#! /usr/bin/python
# -*- coding: utf-8 -*-
'''
Created on 9 avril 2013
@author: benjaminbigot
'''
import sys, os, argparse, re, commands , copy
libSVMPath = '/home/bigot/Developpement/SystemeDeReconnaissanceDuRole_v2/libsvm-3.17/'
import numpy as np
from sklearn.decomposition import PCA
#--------------------------------------------------------------#
def _loadRoleParam(fileName, paramList):
	roleParam = dict()
	
	lines = (line.rstrip('\n') for line in open(fileName))
	paramLinePattern = re.compile('^<role ')

	for line in lines:
		if paramLinePattern.match(line):
			show = line.split(' show="')[1].split('"')[0]
			if show not in roleParam:
				roleParam[show] = dict()
			
			spkName = line.split(' speakerName="')[1].split('"')[0]				
			vecteur = [0.0] * len(paramList)
			for parameter in paramList:
				vecteur[int(paramList[parameter])-1] = line.split(' '+ parameter + '="')[1].split('"')[0]
			roleParam[show][spkName] = vecteur
	# roleParam = [show][speaker] = vecteur_param_ordered
	return roleParam
#--------------------------------------------------------------#
def _loadConfigFile(fileName):
	classifPattern = re.compile('^classifier=')
	rolePattern    = re.compile('^role=')
	paramPattern   = re.compile('^param=')
	
	classif   = dict()
	roleType  = dict()
	paramList = dict()
	
	lines = (line.rstrip('\n') for line in open(fileName))
	for line in lines:
		if classifPattern.match(line):
			idClassif = line.split('"')[1]
			type1     = line.split('"')[3].split(',')
			type2     = line.split('"')[5].split(',')
			fName     = line.split('"')[7]
			classif[idClassif] = dict()
			classif[idClassif]['type1'] = type1
			classif[idClassif]['type2'] = type2
			classif[idClassif]['fName'] = fName
			
		elif rolePattern.match(line):
			idRole = line.split('"')[1]
			role   = line.split('"')[3].split(',')
			roleType[idRole] = role
		
		elif paramPattern.match(line):
			idParam   = line.split('"')[1]
			paramType = line.split('"')[3]
			paramList[paramType] = idParam

	return classif, roleType, paramList
#--------------------------------------------------------------#
def _loadReference(fileName, roleList) :
	roleRef = dict()
	lines = (line.rstrip('\n') for line in open(fileName))
	for line in lines : 
		if line.split()[0] not in roleRef:
			roleRef[line.split()[0]] = dict()
		roleRef[line.split()[0]][line.split()[1]] = line.split()[2]
	return roleRef
#--------------------------------------------------------------#
def _loadLooList(fName):
	looList = []
	lines = (line.rstrip('\n') for line in open(fName))
	for line in lines :
		looList.append(line)	
	return looList
#--------------------------------------------------------------#
def learnClassifier(idClassifier, classifParam, roleReference, roleType, roleParam, listFile):
	''' n step function'''
	print('learning classifier ' + idClassifier)
	# == get role list for the 2 class-problem == #
	role1 = []
	role2 = []

	for roleId in classifParam[idClassifier]['type1'] :
		for i in roleType[roleId]:
			role1.append(i)

	for roleId in classifParam[idClassifier]['type2'] :
		for i in roleType[roleId]:
			role2.append(i)

	# == preparing param vectors == #
	matType1 = []
	matType2 = []
	for fichier in listFile :
		if fichier in roleParam:
			print('=== speakers of ' + fichier + '=====')
			speakers = roleParam[fichier]
			ref      = roleReference[fichier]
			
			for elmt in ref : 
				if (ref[elmt] in role1) and (elmt in speakers) :
					#~ print('adding ' + elmt + ' in type1 ' +ref[elmt] )
					matType1.append(speakers[elmt])
				elif (ref[elmt] in role2) and (elmt in speakers) :
					#~ print('adding ' + elmt + ' in type2 ' + ref[elmt] )
					matType2.append(speakers[elmt] )
				elif (elmt not in speakers):
					print(elmt + ' : no parameters check')
		else :
			print ('no parameters for : ' + fichier)
	
	if matType1 == [] :
		print('type 1 vide')
		return '-1'
	elif matType2 == [] :
		print('type 2 vide')
		return '1'

	
	# == producing PCA from type1 data == #
	X = np.array(matType1, dtype=float)
	X2 =np.array(matType2, dtype=float)
	
	#~ pci = PCA()
	#~ pci.fit(X)
	#~ variance  = pci.explained_variance_ratio_
	#~ cumul = 0
	#~ dim = 0
	#~ for i, v  in enumerate(variance) :
		#~ cumul += v
		#~ print(cumul, i)
		#~ if cumul >= 0.999 : 
			#~ dim = i + 1
			#~ break
	#~ print(dim)
	#~ pca = PCA(n_components=dim)

	pca = PCA(n_components=36)
	pca.fit(X)
	Y = pca.transform(X)
	Y2 = pca.transform(X2)
	matType1 = Y
	matType2 = Y2
	
	# == preparing classification file == #
	outputContent = []
	for line in matType1:
		outLine = '1 '
		for pos, value in enumerate(line):
			outLine += str(pos +1 ) + ':' + str(value) +' '
		outputContent.append(outLine)
	
	for line in matType2:
		outLine = '-1 '
		for pos, value in enumerate(line):
			outLine += str(pos +1 ) + ':' + str(value) +' '
		outputContent.append(outLine)
		
	# == writting to file == #
	fOut = open(classifParam[idClassifier]['fName'] , 'w')
	for line in outputContent:
		fOut.write(line +'\n')
	fOut.close()
	
	# == scaling and learning models == #
	parFile = classifParam[idClassifier]['fName']
	ranged = parFile + '.range'
	model = parFile + '.model'
	scaled = parFile + '.scaled'
	scaling = libSVMPath +'svm-scale -u 1 -l -1 -s ' + ranged + ' ' + parFile + ' > ' + scaled
 	print(scaling)
	out = commands.getoutput(scaling)
	print(out)
	
	training  = libSVMPath  + '/svm-train -t 0 -b 1 ' + scaled + ' ' + model 
	#~ training  = libSVMPath  + '/svm-train  -t 1  ' + scaled + ' ' + model 
	print(training)
	out = commands.getoutput(training)
	print(out)
	return pca
#--------------------------------------------------------------#
def writeTestFile(vect, outputFile, pca):
	ranged = outputFile + '.range'
	model = outputFile + '.model'
	scaled = outputFile + '.test.scaled'
	predictFile = outputFile + '.predict'
	outLine = '0 '

	x = np.empty([1 , len(vect)], dtype=float)
	for i, v in enumerate(vect):
		x[0][i] = v
	Y = pca.transform(x)
	vect = Y
	
	for pos, value in enumerate(vect[0]):
		outLine += str(pos +1 ) + ':' + str(value) +' '
	fOut = open(outputFile + '.test' , 'w')
	fOut.write(outLine + '\n')
	fOut.close()
	
	scaling = libSVMPath +'svm-scale -u 1 -l -1 -r ' + ranged + ' ' + outputFile + '.test' + ' > ' + scaled
 	#~ print(scaling)
	out = commands.getoutput(scaling)
	#~ print(out)
	
	predicting =  libSVMPath +'svm-predict -b 1 ' + scaled +' '+  model + ' ' + predictFile
	#~ predicting =  libSVMPath +'svm-predict ' + scaled +' '+  model + ' ' + predictFile
	#~ print(predicting)
	out = commands.getoutput(predicting)
	#~ print(out)
	
	fRes = open(predictFile, 'r')
	line = fRes.readlines()
	#~ return line[0].rstrip()
	return line[1].split()[0]
#--------------------------------------------------------------#
def recoRole(classif, roleReference, roleType, roleParam, listTest,scoringFile, PCA1, PCA2, PCA3, PCA4):
	''' je contoune le pb sur les cas non resolus '''
	fScoring = open(scoringFile, 'a')
	for fichier in listTest : 
		print(fichier)
		roleTest = roleParam[fichier]
		for speaker in roleTest : 
			if speaker in roleReference[fichier] : 
				print (speaker +' ' +roleReference[fichier][speaker])
				# == preparation du fichier de parametres == #
				#~ print(speaker , roleReference[fichier][speaker], roleParam[fichier][speaker])
			
				# == premier etage classif R1 vs others == #
				if PCA1 != '-1' and PCA1 != '1' :
					predictClass = writeTestFile(roleParam[fichier][speaker], classif['1']['fName'] , PCA1 )
				else : 
					predictClass = PCA1

				if predictClass == '1' :
					print(speaker ,roleReference[fichier][speaker], 'R1')
					fScoring.write(fichier + ' ' + speaker + ' ' + roleReference[fichier][speaker] + ' ' + 'R1'+ '\n')
				
				# == second niveau de classif R2R3 vs R4R5 == #		
				elif predictClass == '-1' :
					
					if PCA2 != '-1' and PCA2 != '1' :
						predictClass = writeTestFile(roleParam[fichier][speaker], classif['2']['fName'] , PCA2)
					else : 
						predictClass = PCA2
				
					# == third level : case R2R3R4R5 == #
					if predictClass == '1' :
					
						# == classif R2 vs R3 == #
						if PCA3 !=  '-1' and PCA3 != '1' :
							predictClass = writeTestFile(roleParam[fichier][speaker], classif['3']['fName'] , PCA3)
						else :
							predictClass = PCA3
						
						if predictClass == '1' :
							print(speaker ,roleReference[fichier][speaker], 'R2')
							fScoring.write(fichier + ' ' + speaker + ' ' + roleReference[fichier][speaker] +' ' +   'R2' + '\n')
						elif predictClass == '-1' :
							print(speaker ,roleReference[fichier][speaker], 'R3')
							fScoring.write(fichier + ' ' + speaker + ' ' + roleReference[fichier][speaker] + ' ' + 'R3'+ '\n')
					
					elif predictClass == '-1' :
						# == classif R4 vs R5 == #
						if PCA4 != '-1'  and PCA4 != '1' :
							predictClass = writeTestFile(roleParam[fichier][speaker], classif['4']['fName'] , PCA4)
						else : 
							predictClass = PCA4
						
						if predictClass == '1' :
							print(speaker ,roleReference[fichier][speaker], 'R4')
							fScoring.write(fichier + ' ' + speaker + ' ' + roleReference[fichier][speaker] +  ' ' +'R4'+ '\n')
						elif predictClass == '-1' :
							print(speaker ,roleReference[fichier][speaker], 'R5')
							fScoring.write(fichier + ' ' + speaker + ' ' + roleReference[fichier][speaker] +  ' ' +'R5'+ '\n')

			else : 
				print ('no reference for : ' +  fichier + ' ' + speaker)
	fScoring.close()
	return 1
#--------------------------------------------------------------#

if __name__=='__main__' :
	parser = argparse.ArgumentParser(description='Main script for speaker role recognition parameter extraction.')
	
	parser.add_argument('-c', '--config'    , dest='configFile', help='configuration file (role def etc...)', required=True)	

	
	parser.add_argument('-p', '--paramFile' , dest='paramFile',  help='main parameter File if l-o-o',     required=False )
	parser.add_argument('-r', '--refFile'   , dest='refFile',    help='main role reference if l-o-o',     required=False)
	
	parser.add_argument('-v', '--appParamFile' , dest='appParamFile',  help='learning parameter File if no l-o-o',     required=False )
	parser.add_argument('-w', '--appRefFile'   , dest='appRefFile',    help='learning role reference if no l-o-o',     required=False)		
	parser.add_argument('-x', '--testParamFile' , dest='testParamFile',  help='test parameter File if no l-o-o',     required=False )
	parser.add_argument('-y', '--testRefFile'   , dest='testRefFile',    help='test role reference if no l-o-o',     required=False)	
	
	parser.add_argument('-l', '--loo'       , dest='looList',    help='learning list if l-o-o', required=False)	
	parser.add_argument('-t', '--listApp'   , dest='listApp',    help='test list if l-o-o', required=False)	
	parser.add_argument('-o', '--outFile', dest='outFile', help='classification  output report', required=True)	
	parser.add_argument('--version', action='version', version='%(prog)s 3.0')
	args = parser.parse_args()
	
	
	
	classif, roleType, paramList 	= _loadConfigFile(args.configFile)
	looList 			= _loadLooList(args.looList)
	
	
	if args.paramFile!=None and args.refFile!=None and args.appParamFile==None and args.appRefFile==None and args.testParamFile==None and args.testRefFile==None :
		
		# == loading param and config files == #
		roleParamApp 						= _loadRoleParam(args.paramFile, paramList)
		roleReferenceApp 					= _loadReference(args.refFile, roleType)
		roleParamTest = roleParamApp
		roleReferenceTest = roleReferenceApp

		
	elif args.paramFile==None and args.refFile==None and args.appParamFile!=None and args.appRefFile!=None and args.testParamFile!=None and args.testRefFile!=None :
		# == loading param and config files == #
		roleParamApp 						= _loadRoleParam(args.appParamFile, paramList)
		roleReferenceApp 					= _loadReference(args.appRefFile, roleType)
		roleParamTest 						= _loadRoleParam(args.testParamFile, paramList)
		roleReferenceTest 					= _loadReference(args.testRefFile, roleType)

		
	# == leave one out == #
	fRes = open( args.outFile, 'w')
	fRes.close()

	if args.listApp != None :
		listApp = _loadLooList(args.listApp)
	else : 
		listApp = copy.copy(looList)
	
	for posTest, nameTest in enumerate(looList):
		listTest = []
		listTrain = []
		print nameTest
		listTest.append(nameTest)
		for posTrain, nameTrain  in enumerate(listApp):
			#if posTest != posTrain:
			if nameTest != nameTrain:
				listTrain.append(nameTrain)
		print listTrain
		
		# == learn classifiers == #
		PCA1 = learnClassifier('1', classif, roleReferenceApp, roleType, roleParamApp, listTrain)
		PCA2 = learnClassifier('2', classif, roleReferenceApp, roleType, roleParamApp, listTrain)
		PCA3 = learnClassifier('3', classif, roleReferenceApp, roleType, roleParamApp, listTrain)
		PCA4 = learnClassifier('4', classif, roleReferenceApp, roleType, roleParamApp, listTrain)
		print(PCA1, PCA2,PCA3,PCA4)
		# == recognition == #
		print('___ recognition_____')
		#~ recoRole(classif, roleReference, roleType, roleParam, listTest, args.outFile, PCA1, PCA2, PCA3, PCA4)
		recoRole(classif, roleReferenceTest, roleType, roleParamTest, listTest, args.outFile, PCA1, PCA2, PCA3, PCA4)


