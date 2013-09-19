'''
Created on 13 dec. 2011

@author: benjaminbigot
'''
import numpy    

#---------------------------------------------------------------------#
def _getSpkNbSegment(spkTurns):
    ''' evaluate the number of speech turns of a given speakers  '''
    return float(len(spkTurns))

#-----------------------------------------------------------------------#
def _getSpeakerTurns(centiMatrix, spkId):
	'''   isolates the speech turns of spkId    and provides a sorted submatrix spkTurns  '''
	spkTurns = []
	for lines in centiMatrix:
		if lines[0] == spkId :
			spkTurns.append(lines)
	return spkTurns    
#----------------------------------------------------------------------#
def _getSpeakerSegmentLength(spkTurns):
	segLength = []
	for line in spkTurns:
		segLength.append((line[2]-line[1])/100.0)
	return segLength
#-------------------------------------------------------------------------------#
def _getSpeakersIntersegmentLength(spkTurns):
	intersegLength = []
	for i in range(1, len(spkTurns)):
		intersegLength.append((spkTurns[i][2] - spkTurns[i-1][1] )/100.0)
	if len(intersegLength)  == 0 :
		intersegLength = [0]
	return intersegLength
#--------------------------------------------------------------------------------------------#	 
def _getMean(vect):      
	return  float("%0.2f"%(numpy.average(vect)))
#--------------------------------------------------------------------------------------------#	 
def _getVar(vect):
	return float("%0.2f"%(numpy.std(vect)))
#--------------------------------------------------------------------------------------------#	 
def _getMin(vect):
	return float(numpy.min(vect))
#--------------------------------------------------------------------------------------------#	 	
def _getMax(vect):
	return float(numpy.max(vect))
#--------------------------------------------------------------------------------------------#
def _getSpan(vect):            
	return  float((vect[-1][2] - vect[0][1])/100.0)
#--------------------------------------------------------------------------------------------#
def _getOvActi(vect):
	return float("%0.2f"%(numpy.sum(_getSpeakerSegmentLength(vect))))
#--------------------------------------------------------------------------------------------#
def _getSoNb(vect):
	return float("%0.2f"%((1.0* _getSpan(vect)) / _getSpkNbSegment(_getSpeakerSegmentLength(vect))))
#--------------------------------------------------------------------------------------------#
def _getAoNb(vect):
	return float("%0.2f"%((1.0* _getOvActi(vect)) / _getSpkNbSegment(_getSpeakerSegmentLength(vect))))
#--------------------------------------------------------------------------------------------#
def _getExtRate(vect):      
	return float("%0.2f"%(   (100.0 * ( _getSpan(vect) - _getOvActi(vect) )) / (1.0 * _getSpan(vect) )))
#--------------------------------------------------------------------------------------------#                    
def extractParam(matrix, spkId) : 
	param = dict()
	subMatrix = _getSpeakerTurns(matrix, spkId)
	segmentLength = _getSpeakerSegmentLength(subMatrix)  
	intersegmentLength = _getSpeakersIntersegmentLength(subMatrix)
	
	param['nbSeg'] 				=  _getSpkNbSegment(subMatrix)
	param['segAvLen'] 			=  _getMean(segmentLength)
	param['segStdLen'] 			=  _getVar(segmentLength)  
	param['segMinLen'] 			=  _getMin(segmentLength)  
	param['segMaxLen'] 			=  _getMax(segmentLength)        
	param['intersegAvLen'] 		=  _getMean(intersegmentLength) 
	param['intersegStdLen'] 	=  _getVar(intersegmentLength)  
	param['intersegMinLength'] 	=  _getMin(intersegmentLength)  
	param['intersegMaxLength'] 	=  _getMax(intersegmentLength)  
	param['span'] 				=  _getSpan(subMatrix)          
	param['overallActivity'] 	=  _getOvActi(subMatrix)        
	param['SoNb'] 				=  _getSoNb(subMatrix)          
	param['AoNb'] 				=  _getAoNb(subMatrix)          
	param['extinctionRate'] 	=  _getExtRate(subMatrix)       
	#~ print(param)
	return param
