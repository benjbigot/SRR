#! /usr/local/bin/python3.2
# -*- coding: utf-8 -*-

import sys,  os, re, time, argparse, datetime  
import extractParamZI
'''
:Date: 10/10/10
:Authors: Ben Bigot            
input are matFile and spklist
output are zi.param and zi.desc files                                                         
'''         
#------------------------------------------------------------------------------------------#
def _validateZone(zone, speakerInvolved):
	''' check the number of speaker involved in the zone
		and the number of turns in the sequence '''
	currentSpeaker = zone[0]
	if len(currentSpeaker) != speakerInvolved:
		return False
	else:
		if zone[2] - zone[1] > 2 :
			return True
		else :
			return False
#------------------------------------------------------------------------------------------#
def _mutualZone(matrix, index, nbSpk, gap):
	''' parie concernant la gestion des segments de non parole : a verifier '''
	speakerInvolved = []
	for j in range(index , len(matrix)) :
		if   (matrix[j][0] != 'NonSpeech')   and  (matrix[j][0] not in speakerInvolved) and  (len(speakerInvolved) < nbSpk) :
			speakerInvolved.append(matrix[j][0])
		elif (matrix[j][0] != 'NonSpeech')   and  (matrix[j][0] not in speakerInvolved) and  (len(speakerInvolved) == nbSpk) :
			return [sorted(speakerInvolved), index , j - 1] 
		elif (matrix[j][0] != 'NonSpeech')   and  (matrix[j][0] in speakerInvolved)	    :
			pass
		elif (matrix[j][0] == 'NonSpeech')   and  (matrix[j][2] - matrix[j][1] <= gap ) :
			pass
		elif (matrix[j][0] == 'NonSpeech')   and  (matrix[j][2] - matrix[j][1]  > gap ) :
			return [sorted(speakerInvolved), index , j - 1] 
			
			
		if ( j == len(matrix) -1 ):
				return [sorted(speakerInvolved), index , j]	
#----------------------------------------------------------------------------------------------------#
def _removeRedundant(zones):
	keptZones = []
	keptZones.append(zones[0])
	for i in range(1, len(zones)):
		if ( zones[i-1][0] == zones[i][0]  and  zones[i-1][1] <= zones[i][1]  and zones[i-1][2] >= zones[i][2]) :
			pass
		else :
			keptZones.append(zones[i])
	return keptZones
#----------------------------------------------------------------------------------------------------#
def _lookingForInteraction(matrixIn, interval, speakerInvolved) :
	''' recherche des zones d'interaction : nombre de locuteurs impliqués et durée de non speech tolérée'''
	interestZone = []	         
	for i in range(len(matrixIn)):         
		# == recherche de la plus longue zone d'interaction entre N locuteurs à partir d'une position i == #
		# zone = [[spk1,spk2],idDebut , idFin ]
		zone 		= _mutualZone(matrixIn, i , speakerInvolved, interval)
				
		# premier test sur le nombre de locuteurs impliqués et la longueur de la zone
		if _validateZone(zone, speakerInvolved):
			interestZone.append(zone)
	#second test = retrait de la redondance.
	keptZone  = _removeRedundant(interestZone)

	
	# third test : removing nonSpeech segments beginning or ending a zone
	for h in keptZone : 
		if matrixIn[ h[2]][0] == 'NonSpeech' :
			h[2] = h[2] - 1

		if matrixIn[ h[1]][0] == 'NonSpeech' :
			h[1] = h[1] + 1 
	
	return keptZone
#---------------------------------------------------------------------------------------------------#
def writeXmlFile(param, outFile, nbSpk):
	
	
	fileContent = []
	fileContent.append('<?xml version="1.0" encoding="UTF-8"?>')
	fileContent.append('<interactionZones author="bbigot" version="0.3" date="' + str(datetime.date.today()) + '"  nbSpk="' + str(nbSpk) + '">')
	for show in param:	
		#~ for zone in param[show]: 
			#~ print zone
		#~ exit()
		
		
		for zone in param[show]: 
			
			#~ print zone
			# == global zone informations == #
			# ------------------------------ #
			line = '<zi show="'+show+'" ' 
			for item in zone:
				if  item != 'speaker' and item != 'silence' and item != 'speechTurns':
					line +=  item +'="'+ str(zone[item]) + '" '


			line += 'starTime="' + str( zone['speechTurns'][0][1]/100.0 ) +'" endTime="' + str( zone['speechTurns'][-1][2]/100.0 ) +'"'
			line += '>'
			fileContent.append(line)
			
			# == speaker informations == #
			# -------------------------- #
			fileContent.append('<speakers>')

			for item in zone['speaker']:

				line ='<speaker name="' + item +'"'
				for val in zone['speaker'][item]:
					line +=  ' '+ val +'="'+ str(zone['speaker'][item][val]) + '"'
				line += '/>'
				fileContent.append(line)
			fileContent.append('</speakers>')

			
			
			# == speech turns informations == #
			# ------------------------------- #
			fileContent.append('<turns>')
			for turn in zone['speechTurns'] :
				line = '<turn ' + 'start="' + str(turn[1]/100.) +'" end="' + str(turn[2]/100.)  + '" speaker="' + turn[0] + '"/>'
				fileContent.append(line)
			fileContent.append('</turns>')
			fileContent.append('</zi>')
			
	fileContent.append('</interactionZones>')
	#~ for i in fileContent:
		#~ print i
	#~ exit()
		
	# == writing to outputFile == #
	# --------------------------- #
	fOut = open(outFile , 'w')
	for line in fileContent : 
		fOut.write(line + '\n')
	fOut.close()	
	return 1
#----------------------------------------------------------------------------------#
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
def _checkOverlappingInteractionZones(currentInteractionZones, matrixIn):
	
	listeIndexToChange = []
	for i in range(0, len(currentInteractionZones)-1):
		if currentInteractionZones[i][2] >= currentInteractionZones[i+1][1] : 
			if (currentInteractionZones[i+1][1] - currentInteractionZones[i][2]) == 0 :
				print 'oveflap de 1'
				listeIndexToChange.append(currentInteractionZones[i+1][1])
			elif (currentInteractionZones[i+1][1] - currentInteractionZones[i][2]) > 0 :
				print 'large overlap not treated so far'
				exit()
	print listeIndexToChange
	
	
	
	for i in currentInteractionZones:
		subsequence = []
		print i 
		for j in range(i[1], i[2]+1):
			subsequence.append(matrixIn[j])
		print subsequence
	
	#~ currentInteractionZones = dict()
	#print interactionZones
	#~ return currentInteractionZones
	return 1
#----------------------------------------------------------------------------------#
def extractInteract(centiMat, nbSpk):
	interactionZones    = dict()
	interactParam       = dict()
	for show in centiMat:
		interactionZones[show] = _lookingForInteraction(centiMat[show], 100, nbSpk) 	
		interactParam[show] = extractParamZI.extractParam(centiMat[show], interactionZones[show])
	return interactParam
#----------------------------------------------------------------------------------#
if __name__=='__main__':
	parser = argparse.ArgumentParser(description='Extract speakers interaction from a mdtm file')
	parser.add_argument('-i', '--input' , dest='inputFile', help='speech description matrix', required=True )
	parser.add_argument('-n', '--nbSpk', dest='nbSpk', help='number of speakers involved', required=True)
	parser.add_argument('-o', '--outXmlFile', dest='outputFile', help='speaker interaction output file', required=True)
	parser.add_argument('--version', action='version', version='%(prog)s 2.0')
	args = parser.parse_args()	

	# == centi mat is a dict indexed by show == #
	centiMatrix 		= _loadCentiMatrix(args.inputFile)
	#print centiMatrix
	#exit()
	# == interactionParam is a dict indexed by show and containing lists == #
	interactionParam 	= extractInteract(centiMatrix,int(args.nbSpk))
	#~ for i in interactionParam:
		#~ for j in interactionParam[i]:
			#~ print j
	#~ exit()
	writeXmlFile(interactionParam, args.outputFile, int(args.nbSpk))

