#! /usr/bin/python
# -*- coding: utf-8 -*-
'''
Created on 12 dec. 2011
@author: benjaminbigot
In this file, temporal acoustic and prosodic parameters are extracted from the speaker clustering and audio
Input info are path to the wavFile, path to the *.mdtm.matSpeechNonSpeechCenti file 
'''
import sys, os, argparse, numpy, extractingInteraction, temporalParam , acousticParam, prosodicParam
import datetime
#~ import scikits.audiolab

#-----------------------------------------------------------------------#
#~ def extractParam(iFile, spkList, wavFile):
	#~ mat = extractingInteraction._loadFile2memory(open(iFile, 'r'))   
	#~ spkId = extractingInteraction._loadSpkIdList(spkList)
	#~ #data, fe = acousticParam._loadingWavFile(wavFile)   
	#~ 
	#~ roleParam = []
	#~ 
	#~ for speaker in spkId : 
		#~ tempoParameters    = temporalParam.extractParam(mat, speaker)
		#~ acousticParameters = acousticParam.extractParam(mat, speaker, wavFile)
		#~ prosodicParameters = prosodicParam.extractParam(mat, speaker, wavFile)
		#~ trame = ('segFile="' 	, iFile 		, '" ' ,\
				 #~ 'speaker="' 	, extractingInteraction._loadSpkNameList(spkList)[int(speaker)] , '" ' ,\
				 #~ 'speakerId="' 	, speaker 		, '" ' ,  tempoParameters, acousticParameters, prosodicParameters)
		#~ #print( ''.join(trame))
		#~ roleParam.append(''.join(trame))
	#~ return roleParam
        #~ 
        
#---------------------------------------------------------#
def uniq(seq, idfun=None): 
	seen = {}
	result = []
	for item in seq:
		marker = item
		# in old Python versions:
		# if seen.has_key(marker)
		# but in new ones:
		if marker in seen: continue
		seen[marker] = 1
		result.append(item)
	return result	        
#------------------------------------------------------------------------#
def getSpeakerList(mat):
	spkList = []
	for line in mat : 
		if line[0] != 'NonSpeech':
			spkList.append(line[0])
	spkListUniq = uniq(spkList)
	return spkListUniq
#------------------------------------------------------------------------#
def extractParam(centiMatrix, wavDir) :
	''' extraction of role parameters '''
	param = dict()
	for show in centiMatrix : 
		# == check wavFile existence == #
		#print wavDir + '/' + show + '.wav'
		wavFile = None
		if os.path.exists(wavDir + '/' + show + '.wav' ) : 
			wavFile = wavDir + '/' + show + '.wav' 
		elif  os.path.exists(wavDir + '/' + show + '.WAV') : 
			wavFile = wavDir + '/' + show + '.WAV'
		else : 
			print('no wavFile found')
			exit()
			
		if wavFile != None : 			
			#~ print wavFile
			param[show] = dict()
			print show
			subMatrix = centiMatrix[show]
			listSpeaker = getSpeakerList(subMatrix)
			for speaker in listSpeaker:
				param[show][speaker] = dict()
				param[show][speaker].update(temporalParam.extractParam(subMatrix, speaker))
				param[show][speaker].update(acousticParam.extractParam(subMatrix, speaker, wavFile))
				param[show][speaker].update(prosodicParam.extractParam(subMatrix, speaker, wavFile))
			
	print param
	return param
	

#------------------------------------------------------------------------#
def _loadCentiMatrix(centiMatFile):
	if os.path.exists(centiMatFile):
		content = (line.rstrip() for line in open(centiMatFile, 'r'))
		centiMat = dict()
		for line in content:
			if len(line.split()) != 4:
				print('check centiMat line ' + centiMatFile +': ' + line)
				exit()
			else : 
				if line.split(' ')[0] not in centiMat:
					centiMat[line.split(' ')[0]] = []
					centiMat[line.split(' ')[0]].append([line.split(' ')[1], int(line.split(' ')[2]), int(line.split(' ')[3]) ])
				else:
					centiMat[line.split(' ')[0]].append([line.split(' ')[1], int(line.split(' ')[2]), int(line.split(' ')[3]) ])
	else: 
		print('Error ' + centiMatFile + ' does not exist... Exiting')
		exit()
		
	return centiMat	
#----------------------------------------------------------------------------------#
def writeXmlFile(param, outFile):
	fileContent = []
	fileContent.append('<?xml version="1.0" encoding="UTF-8"?>')
	fileContent.append('<speakerRole author="bbigot" version="0.3" date="' + str(datetime.date.today()) + '">')
	
	for show in param:	
		for speaker in param[show]: 
			# == global zone informations == #
			# ------------------------------ #
			line = '<role show="'+show+'" speakerName="' + speaker +'" '
			for item in param[show][speaker]:
				line +=  item +'="'+ str(param[show][speaker][item]) + '" '
			line += '/>'
			fileContent.append(line)
			
			
	fileContent.append('</speakerRole>')
	print(fileContent)
	fOut = open(outFile, 'w')
	for line in fileContent:
		fOut.write(line +'\n')
	fOut.close()
	return 1
#----------------------------------------------------------------------------------#

if __name__=='__main__' :
	parser = argparse.ArgumentParser(description='Main script for speaker role recognition parameter extraction.')
	parser.add_argument('-i', '--input' , dest='inputFile', help='centi-second Matrix', required=True )
	parser.add_argument('-w', '--wavFile', dest='wavDir', help='wav directory', required=True)	
	parser.add_argument('-o', '--outXmlFile', dest='outFile', help='param OutFile', required=True)	
	parser.add_argument('--version', action='version', version='%(prog)s 3.0')
	args = parser.parse_args()
	
	centiMatrix = _loadCentiMatrix(args.inputFile)
	param    = extractParam(centiMatrix, args.wavDir)
	writeXmlFile(param, args.outFile)
	
	
