#! /usr/bin/python
# -*- coding: utf-8 -*-
'''
Created on 9 avril 2013
@author: benjaminbigot
'''
import sys, os, argparse, re, commands
#--------------------------------------------------------------#
def editReport(scoringFile, reportFile) :
	
	lines = (line.rstrip('\n') for line in open(scoringFile))
	role = ['R1','R2','R3','R4','R5']
	correct = 0
	faux    = 0 
	
	# == global confusion matrix == #
	roleConfMat = dict()
	for r in role :
		roleConfMat[r]= dict()
	for i in roleConfMat :
		for r in role :
			roleConfMat[i][r] = 0
			
	
	# == confusion matrix by show == #
	showConfMat = dict()
	
	# == confusion matrix by speaker == #
	spkConfMat = dict()
	
	
	for line in lines : 
		show  = line.split()[0]
		speaker  = line.split()[1]
		ref  = line.split()[2]
		hyp  = line.split()[3]
		
		if show not in showConfMat :
			showConfMat[show] = dict()
			for r in role :
				showConfMat[show][r]= dict()
				for i in showConfMat[show] :
					for r in role :
						showConfMat[show][i][r] = 0

		if speaker not in spkConfMat :
			spkConfMat[speaker] = dict()
			for r in role : 
				spkConfMat[speaker][r] = dict()
				for i in spkConfMat[speaker] :
					for r in role :
						spkConfMat[speaker][i][r] = 0


		roleConfMat[ref][hyp] += 1
		showConfMat[show][ref][hyp] += 1
		spkConfMat[speaker][ref][hyp] += 1
		
		if ref == hyp:
			correct += 1
		else : 
			faux += 1
		

	# == scoring == #
	fRes = open(reportFile, 'w')
	fRes.write(str(correct) +' ' + str(faux) +' ' + str(float(correct) / (correct + faux)))
	fRes.close()
	
	printMat(roleConfMat, role, 'roles',reportFile, 'role')

	for show in showConfMat:
		printMat(showConfMat[show], role, show,reportFile, 'show')

	for speaker in spkConfMat:
		printMat(spkConfMat[speaker], role, speaker,reportFile, 'pers')
	
#--------------------------------------------------------------#
def printMat(confMat, role, label,reportFile, typeTag):
	correct = 0
	tous    = 0
	trc     = 0 
	outMat = []
	
	outMat.append(label)
	outMat.append('   |' + ' '.join(role))
	outMat.append('-'*20)
	
	for r in role :
		vect = []
		for m in role :
			if r == m :
				correct += confMat[r][m] 
			tous += confMat[r][m]
			
			vect.append(str(confMat[r][m]))
		outMat.append(r +' |' + '  '.join(vect))
	outMat.append( typeTag + ' trc : ' + str((float(correct)/tous)) + ' ' + label)
	
	fRes = open(reportFile, 'a')
	fRes.write('\n')
	for line in outMat :
		fRes.write(line+'\n')
	fRes.close()
		
#--------------------------------------------------------------#
if __name__=='__main__' :
	parser = argparse.ArgumentParser(description='Main script for speaker role recognition parameter extraction.')
	parser.add_argument('-i', '--inFile', dest='scoreFile', help='classification result', required=True)	
	parser.add_argument('-o', '--outFile', dest='outFile', help='classification report', required=True)	
	parser.add_argument('--version', action='version', version='%(prog)s 3.0')
	args = parser.parse_args()


	# == edit outputReport == #
	editReport(args.scoreFile, args.outFile)
