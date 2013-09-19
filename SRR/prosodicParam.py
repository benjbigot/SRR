#! /usr/local/bin/python2.7
# -*- coding: utf-8 -*-
'''
Created on 14 dec. 2011
@author: benjaminbigot
'''
import sys, os, argparse, scipy, numpy, scikits.audiolab, temporalParam
import commands
import pysox
sys.path.append('/home/bigot/Developpement/SystemeDeReconnaissanceDuRole_v2/srcVOCALIS')
#--------------------------------------------------------# 
def _getSCV(seg, wav):
	''' produce wav and scv segmentation'''
	scvContainer = []
	
	#~ for line in subMatrix :
	# --- preparing audio file --- #
	#commandSox = "sox " + wav + " temp.wav trim " + str( float(line[1]) / 100.0) + " " + str( (float(line[2]) - float(line[1])) / 100.0 ) + ""
	#os.system(commandSox)
	# --- scv segmentation ---#
	#~ os.chdir('srcVOCALIS')
	commandLine = "/home/bigot/Developpement/SystemeDeReconnaissanceDuRole_v2/srcVOCALIS/segSCV -i " + wav + " -s 0.0 -d " + str((float(seg[2]) - float( seg[1]) ) / 100.0) + " -f /home/bigot/Developpement/SystemeDeReconnaissanceDuRole_v2/srcVOCALIS/filt4.dat -w "
	#~ print(commandLine)
	scv =  commands.getoutput(commandLine).split('\n')
	#~ print(len(scv))
	#~ print(scv)
	try :
		for x in scv :
			scvContainer.append([float(x.split()[0])  , float(x.split()[1]) , (x.split()[2])])
	except :
		scvContainer.append([0.0, 0.1 , '#'])
		
		#scvContainer.append(x)
	#~ os.chdir('..')
	#~ scvTruc = numpy.asarray(scvContainer)
	#print(scvContainer)
	#print(scvTruc)
	#exit()
	#~ return scvTruc
	return scvContainer
#----------------------------------------------------------#	
def _getPitch(seg, wav):
	''' produce pitch values '''
	pitchContainer = []
	commandPitch = "aubiopitch -m yin -i "+ wav +" -l 40 -M 400"
	pitch = commands.getoutput(commandPitch).split('\n')
	for x in pitch :
		pitchContainer.append([float(x.split('\t')[0])  , float(x.split('\t')[1])  ])
	return pitchContainer
	
	#~ for line in subMatrix :
	# --- preparing audio file --- #
	#~ commandSox = "sox " + wav + " temp.wav trim " + str( float(line[1]) / 100.0) + " " + str( (float(line[2]) - float(line[1])) / 100.0 ) + ""
	#~ os.system(commandSox)
	# --- pitch extraction --- #
	#~ pitchTruc = numpy.asarray(pitchContainer)
	#print(pitchTruc)
	#~ return pitchTruc
	#~ print( pitchContainer)
	
#---------------------------------------------------------#
def _getVowel(scvContainer) :
	''' from scv result to vowel duration list '''
	vowel = []
	
	for line in scvContainer:
		if line[2] == 'V' :
			vowel.append(float( line[1] ) -  float( line[0]) )
	if not vowel :
			vowel=[0]
	return vowel
#---------------------------------------------------------#
def _getSilence(scvContainer) :
	''' from scv result to silence duration list '''
	silence = []
	
	for line in scvContainer:
		if line[2] == '#' :
			silence.append(float( line[1] ) -  float( line[0]) )
	if not silence : 
		silence=[0]
	#print(silence)
	return silence
#--------------------------------------------------------# 
def _getPitched(pitchContainer) :
	pitch = []

		
	for line in pitchContainer :
		if ( (float(line[1]) != -1.0) and (float(line[1]) != 16000.0 ) ):
			pitch.append(float(line[1]))
	#~ print(pitch)
	if not pitch :
			pitch=[0]
	return pitch
#--------------------------------------------------------#
def _getPitchedNb(pitchContainer) :
	nbZone = 0
	prevStamp = -1.0
	
	for line in pitchContainer :
		if ((( float(line[1]) != -1.0 ) and (float(line[1]) != 16000.0)) and ((prevStamp == -1.0) or (prevStamp  == 16000.0 ))) :
			nbZone +=1
			prevStamp = float(line[1])
		else :
			prevStamp = float(line[1])			
	return nbZone
#--------------------------------------------------------# 
def _getAvgPitch(pitchContainer) :
	return numpy.average(_getPitched(pitchContainer))
#--------------------------------------------------------#
def _getStdPitch(pitchContainer) : 
	return numpy.std(_getPitched(pitchContainer))
#--------------------------------------------------------#
def _getMaxPitch(pitchContainer):
	return numpy.max(_getPitched(pitchContainer))
#--------------------------------------------------------#
def _getVoicedZoneRate(pitchNb,activity) :
	return (float(pitchNb) / float(activity))
#--------------------------------------------------------#
def _getNbVocalic(vowel):
	return (len(vowel))
#--------------------------------------------------------#
def _getRateVocalic(vowel, activity) :
	return (float(len(vowel)) /float(activity))
#--------------------------------------------------------#
def _getAvgVocalicLen(vowel) :
	return numpy.average(vowel)
#--------------------------------------------------------#
def _getStdVocalicLen(vowel) : 
	return numpy.std(vowel)
#--------------------------------------------------------#
def _getNbSilence(silence) :
	return (len(silence))
#--------------------------------------------------------#
def _getRateSilence(silence, activity) :
	return (float(len(silence))/float(activity))
#--------------------------------------------------------#
def _getAvgSilenceLen(silence):
	return numpy.average(silence)
#--------------------------------------------------------# 
def _getStdSilenceLen(silence):
	return numpy.std(silence)
#--------------------------------------------------------# 
def _getSpeakerTurns(centiMatrix, spkId):
	'''   isolates the speech turns of spkId    and provides a sorted submatrix spkTurns  '''
	spkTurns = []
	for lines in centiMatrix:
		if lines[0] == spkId :
			spkTurns.append(lines)
	return spkTurns    
#--------------------------------------------------------#
def _loadingWavFile(wav):
	'''  exploits numpy to obtain a numeric vector correspodning to the audiofile. '''
	data,fe,enc = scikits.audiolab.wavread(wav)
	return data, fe                                         
#-------------------------------------------------------#
def extractParam(matrice, spkId, wavFile):
	spkTurns = _getSpeakerTurns(matrice, spkId)
	#scvContainer = numpy.empty((0)) 
	scvContainer = []
	pitchContainer = []
	
	for seg in spkTurns :
		tempFile = wavFile.split('.wav')[0] +'_'+ str(seg[0]) +'_'+ str(seg[1]) +'_'+ str(seg[2]) + '.wav'
		print(tempFile)
		# == trim sox via pysox == #
		infile = pysox.CSoxStream(wavFile)
		outfile = pysox.CSoxStream(tempFile,'w',infile.get_signal())
		chain = pysox.CEffectsChain(infile, outfile)
		effect = pysox.CEffect('trim', [ str(seg[1] / 100. ), str( (seg[2]-seg[1])/100.) ])
		chain.add_effect(effect) 
		chain.flow_effects()
		outfile.close()

		scvContainer 	+= _getSCV(seg, tempFile)
		pitchContainer 	+= _getPitch(spkTurns, tempFile)
		os.remove(tempFile)

	
	pitchContainer 	= numpy.asarray(pitchContainer)
	scvContainer 	= numpy.asarray(scvContainer)
	
	#_getSpkPitch(spkTurns, wavFile)
	#print(_getPitched(pitchContainer))
	#print(_getPitchedNb(pitchContainer))	
	#print(_getVowel(scvContainer))
	#print(_getSilence(scvContainer))
	#print(temporalParam._getOvActi(spkTurns))
	#~ scvContainer = _getSCV(spkTurns, wavFile)
	#~ pitchContainer = _getPitch(spkTurns, wavFile)
	spkTrame = dict()
	
	spkTrame['avgPitch']		= float("%0.3e"%(_getAvgPitch(pitchContainer)))	
	spkTrame['stdPitch']		= float("%0.3e"%(_getStdPitch(pitchContainer)))	
	spkTrame['maxPitch']		= float("%0.3e"%(_getMaxPitch(pitchContainer)))	
	spkTrame['voicedZoneRate']	= float("%0.3e"%(_getVoicedZoneRate(_getPitchedNb(pitchContainer),temporalParam._getOvActi(spkTurns))))	
	spkTrame['nbVocalic']		= float("%0.3e"%(_getNbVocalic(_getVowel(scvContainer))))					
	spkTrame['rateVocalic']		= float("%0.3e"%(_getRateVocalic(_getVowel(scvContainer) , temporalParam._getOvActi(spkTurns)    )))	
	spkTrame['avgVocalicLen']	= float("%0.3e"%(_getAvgVocalicLen(_getVowel(scvContainer))))		
	spkTrame['stdVocalicLen']	= float("%0.3e"%(_getStdVocalicLen(_getVowel(scvContainer))))		
	spkTrame['nbSilence']		= float("%0.3e"%(_getNbSilence(_getSilence(scvContainer))))		
	spkTrame['rateSilence']		= float("%0.3e"%(_getRateSilence(_getSilence(scvContainer) , temporalParam._getOvActi(spkTurns)  )))		
	spkTrame['avgSilenceLen']	= float("%0.3e"%(_getAvgSilenceLen(_getSilence(scvContainer))))		
	spkTrame['stdSilenceLen']	= float("%0.3e"%(_getStdSilenceLen(_getSilence(scvContainer))))		
	#~ print(spkTrame)
	return spkTrame

	
	
