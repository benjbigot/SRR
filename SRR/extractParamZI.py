#! /usr/local/bin/python3.2
# -*- coding: utf-8 -*-

import sys,  os, re, time, numpy
from collections import defaultdict

'''
:Date: 16/08/2012
:Authors: Ben Bigot            
zi parameters extraction module'''

#-------------------------------------------------------#
def _getInteractionLevel(interactZone, submatrix):
	''' does not really take into account the number of UI for every couple of speakers'''

	# == interaction automata == #
	speakerList  = interactZone[0]
	interactMap = dict()
	for i in range(len(speakerList) - 1):
		for j in range( i+1 , len(speakerList)):
			interactMap[speakerList[i] + ' ' + speakerList[j]] = 0
	
	# == speech urn sequence == #
	speakerSeq = []
	for line in submatrix :
		if (len(speakerSeq) == 0) and (line[0] != 'NonSpeech') :
			speakerSeq.append(line[0])
		elif (len(speakerSeq) > 0) and (line[0] != 'NonSpeech') and (line[0] != speakerSeq[-1]) :
			speakerSeq.append(line[0])
	
	# == automata validation using speech tun sequence == #
	for i in range(1, len(speakerSeq)) :
		if speakerSeq[i-1] + ' ' + speakerSeq[i] in interactMap:
			interactMap[ speakerSeq[i-1] + ' ' + speakerSeq[i] ] = 1
		elif speakerSeq[i] + ' ' + speakerSeq[i-1] in interactMap:
			interactMap[ speakerSeq[i] + ' ' + speakerSeq[i-1] ] = 1
	

	# == interaction needed between every one, else IL = 0)
	for i in interactMap:
		if interactMap[i] == 0:
			return 0

	
	nbUI = len(speakerSeq) - 1
	return (nbUI - 1)
#---------------------------------------------------#
def _getInteractionDuration(desc, submatrix):
	return (submatrix[-1][2] - submatrix[0][1])/100.0
#---------------------------------------------------#
def _getInteractionDynamic(level, duration):
	return "%0.2f"%(100.0*level/duration)          
#---------------------------------------------------#
def _getLengthParam(param, desc, submatrix): 
	''' calculate average and std of speaker segment on the zone '''

	#~ l=lambda:defaultdict(l)
	#~ spk=l()
	param['speaker'] = dict()
		
	for speaker in desc[0] : 
		param['speaker'][speaker] = dict()
		temp = []
		for line in submatrix:
			if line[0] == speaker : 
				temp.append((line[2] - line[1])/100.0)
		param['speaker'][speaker]['avgLength'] = "%0.2f"%(numpy.average(temp))
		param['speaker'][speaker]['stdLength']     = "%0.2f"%(numpy.std(temp)) 
		param['speaker'][speaker]['duration']  = "%0.2f"%(numpy.sum(temp)) 
		param['speaker'][speaker]['counterSeg'] = len(temp)    
	
		
	temp=[]	
	param['speaker']['silence'] = dict()
	for line in submatrix :
		if line[0] == 'NonSpeech':
			temp.append((line[2] - line[1])/100.0)
	if len(temp) > 0 :
		param['speaker']['silence']['avgLength'] = "%0.2f"%(numpy.average(temp)) 
		param['speaker']['silence']['stdLength']     = "%0.2f"%(numpy.std(temp)) 
		param['speaker']['silence']['duration']  = "%0.2f"%(numpy.sum(temp)) 
		param['speaker']['silence']['counterSeg'] = len(temp)
	else :
		param['speaker']['silence']['avgLength'] = 0
		param['speaker']['silence']['stdLength']     = 0
		param['speaker']['silence']['duration']  = 0
		param['speaker']['silence']['counterSeg'] = 0
			
	return param 
#---------------------------------------------------#
def _getSpeakerContribution(desc, param):
	cumulateSpeech = 0;
	for speaker in desc[0]:
		cumulateSpeech +=  float(param['speaker'][speaker]['duration'])

	for speaker in desc[0] :
		param['speaker'][speaker]['contrib'] = "%0.2f"%(100.0 * float(param['speaker'][speaker]['duration'])/(cumulateSpeech)) 
	return param
#---------------------------------------------------# 
#~ def _prepareOut(param, desc, matrix, level, duration , dynamic, spkList):
	#~ nbSegment=[] ; average = [] ; std =[] ; length = [] ;loc = [] ; contrib = [] ; spkId =[]
	#~ for speaker in desc[0]:
		#~ loc.append(str(speaker))
		#~ contrib.append(str(param[speaker]['contrib']))
		#~ nbSegment.append(str(param[speaker]['counter']))
		#~ average.append(str(param[speaker]['average']))
		#~ std.append(str(param[speaker]['std']))
		#~ length.append(str(param[speaker]['length']))
		#~ spkId.append(spkList[speaker])
		#~ 
	#~ line = 'startTime="'	,  	matrix[0][1] 		,\
			#~ '" endTime="'	,  	matrix[-1][2]		,\
			#~ '" speakers="'	, 	' '.join(loc) 			,\
			#~ '" spkName="'	, 	' '.join(spkId) 			,\
			#~ '" level="'		,  	level				,\
			#~ '" duration="' 	,   duration			,\
			#~ '" dynamic="' 	,	dynamic	 	     	,\
			#~ '" spkContrib="' 	,	' '.join(contrib)	 	     	,\
			#~ '" nbSegment="'	, 	' '.join(nbSegment) ,\
			#~ '" spkDuration="',	' '.join(length) 	,\
			#~ '" spkAvg="'		,	' '.join(average)	,\
			#~ '" spkStd="' 	,	' '.join(std)		,\
			#~ '" nonSpeechAvg="'		,	param['silence']['average']	,\
			#~ '" nonSpeechStd="'		,	param['silence']['std']		,\
			#~ '" nonSpeechDuration="',	param['silence']['length']	,\
			#~ '" nonSpeechSegment="'	,	param['silence']['counter'] ,'"'
	#~ 
	#~ return ''.join(map(str,line) )
#----------------------------------------------------#
#def extractParam(matrix, interaction, spkList):
def extractParam(matrix, interaction):
	''' c'est la zone qui est caracterisée pas le locuteur '''
	output = []
	for zone in interaction :
		param = dict()
		param['iLevel']  = _getInteractionLevel(zone, matrix[zone[1]: zone[2]+1])
		param['idStart'] = zone[1]
		param['idEnd']   = zone[2]
			
		if param['iLevel'] > 0 :
			param['iDuration']  = _getInteractionDuration( zone, matrix[zone[1]: zone[2]+1] )
			param['iDynamic']   = _getInteractionDynamic(param['iLevel'] , param['iDuration'] )
			param			 	= _getLengthParam(param,  zone, matrix[zone[1]: zone[2]+1] )
			param				= _getSpeakerContribution(zone, param)     
			output.append(param)
	return output
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
