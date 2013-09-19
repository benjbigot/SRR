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
	''' check the number of speaker involved in the zone'''
	currentSpeaker = zone[0]
	if len(currentSpeaker) != speakerInvolved:
		return False
	else: 
		return True
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
		
		if _validateZone(zone, speakerInvolved):
			interestZone.append(zone)
	keptZone  = _removeRedundant(interestZone)
	return keptZone
#---------------------------------------------------------------------------------------------------#
def writeXmlFile(param, matrix, outFile, nbSpk):
	fileContent = []
	fileContent.append('<?xml version="1.0" encoding="UTF-8"?>')
	fileContent.append('<interactionZones author="bbigot" version="0.3" date="' + str(datetime.date.today()) + '"  nbSpk="' + str(nbSpk) + '">')
	for show in param:	
		for zone in param[show]: 
			# == global zone informations == #
			# ------------------------------ #
			line = '<zi show="'+show+'" ' 
			for item in zone:
				if  item != 'speaker' and item != 'silence':
					line +=  item +'="'+ str(zone[item]) + '" '
			line += 'starTime="' + str( matrix[show][int(zone['idStart'])][1] /100.0 ) +'" endTime="' + str( matrix[show][int(zone['idEnd'])][2] /100.0 ) +'"'
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
			for turn in matrix[show][ zone['idStart']:  zone['idEnd']+1 ] :
				line = '<turn ' + 'start="' + str(turn[1]/100.) +'" end="' + str(turn[2]/100.)  + '" speaker="' + turn[0] + '"/>'
				fileContent.append(line)
			fileContent.append('</turns>')
			fileContent.append('</zi>')
	fileContent.append('</interactionZones>')
		
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
def extractInteract(centiMat, nbSpk):
	interactionZones = dict()
	interactParam    = dict()
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
	# == interactionParam is a dict indexed by show and containing lists == #
	interactionParam 	= extractInteract(centiMatrix,int(args.nbSpk))
	writeXmlFile(interactionParam, centiMatrix, args.outputFile, int(args.nbSpk))

