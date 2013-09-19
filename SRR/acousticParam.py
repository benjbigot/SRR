'''
Created on 13 dec. 2011

@author: benjaminbigot
'''
import numpy
import scipy
import scikits.audiolab
import temporalParam
import pysox
import os
#~ import matplotlib.pyplot as plt

#-----------------------------------------------------------------------#
def _getSpeakerTurns(centiMatrix, spkId):
	'''   isolates the speech turns of spkId    and provides a sorted submatrix spkTurns  '''
	spkTurns = []
	for lines in centiMatrix:
		if lines[0] == spkId :
			spkTurns.append(lines)
	return spkTurns    
#----------------------------------------------------------------------#
def _loadingWavFile(wav):
	'''  exploits numpy to obtain a numeric vector correspodning to the audiofile. '''
	data,fe,enc = scikits.audiolab.wavread(wav)
	#~ plt.plot(data)
	#~ plt.show()
	return data, fe                                         
#-------------------------------------------------------#
def _getShortTimeEnergy(data, fe, winSize, winStep ):
	energy = numpy.square(data)
	shortTimeEnergy     = numpy.zeros((len(energy)/winStep))
	shortTimeEnergy[0]  = numpy.average(energy[0:winStep])           		
	shortTimeEnergy[-1] = numpy.average(energy[ len(energy) - winStep  : len(energy)])		
	for idBegin in range (winStep,  len(energy)- winSize , winStep):
		shortTimeEnergy[idBegin/winStep] = numpy.average(energy[idBegin:(idBegin+winSize)])
	return shortTimeEnergy
#--------------------------------------------------------#       
def _getStdPowerSignal(signal):
	return numpy.std(signal)
#--------------------------------------------------------#       
def _getAvgPowerSignal(signal):
	return numpy.average(signal)
#--------------------------------------------------------# 
def _getAvgLowPowerSignal(signal):
	return numpy.average(signal[(signal < (0.15 * _getAvgPowerSignal(signal)))])
#--------------------------------------------------------# 
def _getStdLowPowerSignal(signal):
	return numpy.std(signal[(signal < (0.15 * _getAvgPowerSignal(signal)))])
#--------------------------------------------------------# 
def _getMinLowPowerSignal(signal):
	return numpy.min(signal[(signal < (0.15 * _getAvgPowerSignal(signal)))])
#--------------------------------------------------------# 
def _getMaxLowPowerSignal(signal):
	return numpy.max(signal[(signal < (0.15 * _getAvgPowerSignal(signal)))])
#--------------------------------------------------------# 
def _getAvgHighPowerSignal(signal):
	return numpy.average(signal[(signal >= (0.15 * _getAvgPowerSignal(signal)))])
#--------------------------------------------------------# 
def _getStdHighPowerSignal(signal):
	return numpy.std(signal[(signal >= (0.15 * _getAvgPowerSignal(signal)))])
#--------------------------------------------------------# 
def _getMinHighPowerSignal(signal):
	return numpy.min(signal[(signal >= (0.15 * _getAvgPowerSignal(signal)))])
#--------------------------------------------------------# 
def _getMaxHighPowerSignal(signal):
	return numpy.max(signal[(signal >= (0.15 * _getAvgPowerSignal(signal)))])
#--------------------------------------------------------# 

def extractParam(matrice, spkId,wavFile) :
	''' analyse sur des fenetres de 10ms toutes les 5 ms -> nb echantillons entiers '''
	spkTurns = _getSpeakerTurns(matrice, spkId)
	shortTimeEnergyContainer = numpy.empty((0)) 

	#~ windowSize = 10  ; 	stepSize = 5 ; # in ms	
	for seg in spkTurns :
		tempFile = wavFile.split('.wav')[0] +'_'+ str(seg[0]) +'_'+ str(seg[1]) +'_'+ str(seg[2]) + '.wav'
		#~ print(tempFile)
		
		# == trim sox via pysox == #
		infile = pysox.CSoxStream(wavFile)
		outfile = pysox.CSoxStream(tempFile,'w',infile.get_signal())
		chain = pysox.CEffectsChain(infile, outfile)
		effect = pysox.CEffect('trim', [ str(seg[1] / 100. ), str( (seg[2]-seg[1])/100.) ])
		chain.add_effect(effect) 
		chain.flow_effects()
		outfile.close()
		
		# == loading segment == #
		wavData, fe  = _loadingWavFile(tempFile)
		windowSizeSample = 10 * fe /1000
		stepSizeSample = 5 * fe / 1000

		# == computing and concatening short time enerfy == #
		shortTimeEnergy = _getShortTimeEnergy(wavData, fe, windowSizeSample, stepSizeSample)    	
		shortTimeEnergyContainer = numpy.concatenate((shortTimeEnergyContainer , shortTimeEnergy))
		os.remove(tempFile)

	spkTrame = dict()
	spkTrame['avgPowerSignal']		= float("%0.3e"%(_getAvgPowerSignal(shortTimeEnergy)))	
	spkTrame['stdPowerSignal']		= float("%0.3e"%(_getStdPowerSignal(shortTimeEnergy)))	
	spkTrame['avgLowPowerSignal']	= float("%0.3e"%(_getAvgLowPowerSignal(shortTimeEnergy))) 
	spkTrame['stdLowPowerSignal']	= float("%0.3e"%(_getStdLowPowerSignal(shortTimeEnergy))) 
	spkTrame['minLowPowerSignal']	= float("%0.3e"%(_getMinLowPowerSignal(shortTimeEnergy))) 
	spkTrame['maxLowPowerSignal']	= float("%0.3e"%(_getMaxLowPowerSignal(shortTimeEnergy))) 
	spkTrame['avgHighPowerSignal']	= float("%0.3e"%(_getAvgHighPowerSignal(shortTimeEnergy)))
	spkTrame['stdHighPowerSignal']	= float("%0.3e"%(_getStdHighPowerSignal(shortTimeEnergy)))
	spkTrame['minHighPowerSignal']	= float("%0.3e"%(_getMinHighPowerSignal(shortTimeEnergy)))
	spkTrame['maxHighPowerSignal']	= float("%0.3e"%(_getMaxHighPowerSignal(shortTimeEnergy)))
	
	return spkTrame
	
