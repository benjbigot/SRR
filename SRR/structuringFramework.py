#! /usr/bin/python
# -*- coding: utf-8 -*-
'''
Created on 4 April 2013
@author: benjaminbigot
Launching extraction of interaction zones and role parameter from a speaker diarization file
'''
import sys, os, argparse
import mdtm2mat, extractingInteraction , extractingRoleParameters#, prepareParameters
#======================================================================#

if __name__=='__main__':  	
	parser = argparse.ArgumentParser(description='From Speaker Diarization result and wavfile to SVM input')
	parser.add_argument('-m', '--input' , dest='mdtmFile', help='Speaker Diarization input file (mdtm)', required=True )
	parser.add_argument('-w', '--wavDir', dest='wavDir', help='wav files Dir', required=True)
	parser.add_argument('-c', '--config', dest='configFile', help='configuration file', required=True)
	parser.add_argument('-o', '--output', dest='outputDir', help='SVM parameters file', required=True)
	parser.add_argument('--version', action='version', version='%(prog)s 3.0')
	args = parser.parse_args()
	
	print('_______preparing matrix_______')
	# format {'show':[[name, start, end], [name, start, end]], 'show2':[[name, start, end], [name, start, end]]}
	centiMatrix = mdtm2mat.mdtm2mat(args.mdtmFile)

	print('_______characterizing interactions________')
	interactionList = extractingInteraction.extractInteract(centiMatrix,2)
	# ==  may print the number of zi found == #
	# == pourrai faire appel à la fonction 
	# == extractingInteraction.writeXmlFile(interactionParam, centiMatrix, args.outputFile, int(args.nbSpk))

	print('_______parameters extraction________')
	roleP = extractingRoleParameters.extractParam(centiMatrix, args.wavDir)
	
	#~ print("ouput preparation")
	#~ prepareParameters.writingParameters(roleP, args.configFile, args.outputDir )
	#~ outName = outputDir + '.spkList'
	#~ spkListOut = open(outName , "w")
	#~ for i in spkNameList :
		#~ spkListOut.write(i + '\n')
	#~ spkListOut.close()
