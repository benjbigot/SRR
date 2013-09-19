#! /usr/bin/python
# -*- coding: utf-8 -*-
'''
Created on 9 avril 2013
@author: benjaminbigot
'''

import sys, os, argparse, re
#---------------------------------------------------#
def loadRef(refFile):
	refRole = dict()
	lines = (line.rstrip('\n') for line in open(refFile))
	for i in lines :
		show = i.split()[0]
		spk  = i.split()[1]
		role = i.split()[2]
		
		if show not in refRole:
			refRole[show] = dict()
		refRole[show][spk] = role
	return refRole
#---------------------------------------------------#
def getMapping(mapFile, refRole):
	mapping = dict()
	lines = (line.rstrip('\n') for line in open(mapFile))

	for i in lines :
		if re.match(".* processing .*", i):
			show = i.split(' ')[4].replace(',', '')
			if show not in mapping:
				mapping[show] = dict()
		if re.match(".* => .*", i):
			if not re.match(".* <nil>", i) :
				print('match', i)
				name = i.split('\'')[1]
				print name
			
				eq = i.split('\'')[3]
				print('eq',  eq)
				mapping[show][name] = refRole[show][eq]
				print(eq, name, mapping[show][name])
			else : 
				print('non',  i)
			
	return mapping
#---------------------------------------------------#
def writeMapping(Rmapping, outFile):
	fOut = open(outFile, 'w')
	for show in Rmapping:
		for loc in Rmapping[show]:
			fOut.write(show + ' ' + loc + ' ' + Rmapping[show][loc] +'\n')
	fOut.close()
#---------------------------------------------------#
def mapRef2Auto(ref, mapping, outFile):
	refRole = loadRef(ref)
	Rmapping = getMapping(mapping, refRole)
	writeMapping(Rmapping, outFile)
#---------------------------------------------------#
if __name__=='__main__' :
	parser = argparse.ArgumentParser(description='Main script for speaker role recognition parameter extraction.')
	parser.add_argument('-r', '--refFile' 	,dest='refFile'		,help='RoleRefFile',    	 	required=True )	
	parser.add_argument('-m', '--mapping'   ,dest='mappingFile'	,help='mapping file', 			required=True)	
	parser.add_argument('-o', '--outFile'	,dest='outFile'		,help='auto role ground truth', required=True)	
	parser.add_argument('--version', action='version', version='%(prog)s 3.0')
	args = parser.parse_args()


	mapRef2Auto(args.refFile, args.mappingFile, args.outFile)
