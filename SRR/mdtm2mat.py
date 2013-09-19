#! /usr/bin/python
# -*- coding: utf-8 -*-

import sys, os, argparse

'''
:Date: 10/10/10
:Authors: Ben Bigot
mdtm2mat.py generates 
- one file containing the speakers interventions matrix
- one file containing information relative to speakers characterization             
'''                         
#----------------------------------------------------------------------#
def _writeMatrix(mdtmMatrixCentiSecond, outputFile ):
	''' write output matrix to text file '''
	fileOutNameList = open( outputFile , 'w')
	for show in mdtmMatrixCentiSecond:
		for i in mdtmMatrixCentiSecond[show]:
			fileOutNameList.write(show + ' ' + i[0]+' '+str(i[1])+' '+str(i[2])+'\n')
	fileOutNameList.close()
	return 1
#------------------------------------------------------------------------#       
def _writeSpkList(spkList, ofile, ifile):
    '''  writeXML generate an output file containing information relative to speakers  '''
    fileOutNameList = open(ofile, 'w')
    fileOutNameList.write('<SpkNameList inputFile="' + ifile + '" outputFile="' + ofile + '">\n')
    for i in spkList:
        fileOutNameList.write('<speaker spkName="'+i+'" spkId="'+str(spkList.index(i))+'"></speaker>\n')
    fileOutNameList.write('</SpkNameList>\n')
    fileOutNameList.close()
    return 1
#---------------------------------------------------------#
def  _addNonSpeechEvents(startEndMatrix):
	'''  add the non speech non overlapping segments separating two speakers interventions.
    Try to avoid overlapping segments as well    '''

	nonSpeechMatrix =[]
	coveringSegmentCnt = 0

	for i in range(len(startEndMatrix)-1):
		if (startEndMatrix[i][1] == startEndMatrix[i+1][1]) and (startEndMatrix[i][2] == startEndMatrix[i+1][2]):
			coveringSegmentCnt += 1
			
		elif (startEndMatrix[i][2])+1 < startEndMatrix[i+1][1]:   
			nonSpeechMatrix.append(['NonSpeech', startEndMatrix[i][2]+1 ,startEndMatrix[i+1][1]-1 ])
	
	nonSpeechMatrix.extend(startEndMatrix)
	nonSpeechMatrix.sort(key = lambda x : x[1] )
	
	print("overlapping segments found : ", coveringSegmentCnt)
	return nonSpeechMatrix
#-------------------------------------------------------#
def _concatSegments(mdtmMatrixIn, seuil):
    '''  Two segments attributed to the same speaker, and separated by n (or less) centi-seconds are concatenated.    '''
    for i in range( len(mdtmMatrixIn) -1 ):
        if (mdtmMatrixIn[i][0] == mdtmMatrixIn[i+1][0]):
            if ((mdtmMatrixIn[i+1][1]- mdtmMatrixIn[i][2] + 1) <= seuil):
                mdtmMatrixIn[i+1][1] = mdtmMatrixIn[i][1]
                mdtmMatrixIn[i] = [-1 , -1 , -1]
    
    mdtmMatrixIn.sort(key = lambda x : x[1] )
    mdtmMatrixConcat = []
    for line in mdtmMatrixIn:
        if line != [-1 , -1 , -1]:
            mdtmMatrixConcat.append(line)
            
    return mdtmMatrixConcat	
#--------------------------------------------------------#
def _getSpkList(matrix)	:
	''' produce a speaker list indexed by speakers position '''
	spkList = []
	for line in matrix:
		if line.split()[7] not in spkList :
			spkList.append(line.split()[7])
    #      
	return spkList
#--------------------------------------------------------#
def _loadMDTM(fileName):
	'''   load and check mdtm file, return fileContent array 	'''
	if os.path.exists(fileName):
		
		#~ fileIn = open(fileName, "r")
		#~ content = fileIn.readlines()
		content = (line.rstrip() for line in open(fileName, 'r'))
		#~ fileIn.close()
		
		
		centiMat = dict()
		for line in content:
			if len(line.split(' ')) != 8 :
				print('check mdtmFile :' + line + ' in ' + fileName)
				print('Exiting')
				exit()
			else : 
				if line.split(' ')[0] not in centiMat:
					centiMat[line.split(' ')[0]] = []
					centiMat[line.split(' ')[0]].append([line.split(' ')[7], int(100 * float(line.split(' ')[2])), int(100 * float(line.split(' ')[2]))  + int(100 * float(line.split(' ')[3])) ])
				else:
					centiMat[line.split(' ')[0]].append([line.split(' ')[7], int(100 * float(line.split(' ')[2])), int(100 * float(line.split(' ')[2]))  + int(100 * float(line.split(' ')[3])) ])
	else:
		print(fileName + 'does not exist... Exiting')
		exit()
		
		
		
	return centiMat
#--------------------------------------------------------#
def mdtm2mat(inputMDTM): # , listeOut, matrixOut) :
	
	# == renvoi une structure indexée par le nom du show == #

	centiMat = _loadMDTM(inputMDTM)

	centiMatUniq = dict()
	centiMatConcat = dict()
	centiMat_S_NS  = dict()
	
	for show in centiMat:
		print(show)
		centiMatUniq[show] = [list(x) for x in set(tuple(x) for x in centiMat[show])] 
		centiMatUniq[show].sort(key = lambda x : x[1])
		centiMatConcat[show]  	= _concatSegments(centiMatUniq[show], 2)
		centiMat_S_NS[show]		= _addNonSpeechEvents(centiMatConcat[show])
	
	return centiMat_S_NS
#--------------------------------------------------------#
if __name__=='__main__':
	parser = argparse.ArgumentParser(description='From Speaker Diarization result to speakerList and audio event matrix.')
	parser.add_argument('-i', '--input' , dest='inputFile', help='Speaker Diarization input file (mdtm)', required=True )
	parser.add_argument('-o', '--outMatrix', dest='outputMatrix', help='centi matrix', required=True)
	parser.add_argument('--version', action='version', version='%(prog)s 3.0')
	args = parser.parse_args()
	
	centiMat = mdtm2mat(args.inputFile)
	_writeMatrix(centiMat, args.outputMatrix)
#---------------------------------------------------------#

#~ def uniq(seq, idfun=None): 
	#~ seen = {}
	#~ result = []
	#~ for item in seq:
		#~ marker = item
		#~ # in old Python versions:
		#~ # if seen.has_key(marker)
		#~ # but in new ones:
		#~ if marker in seen: continue
		#~ seen[marker] = 1
		#~ result.append(item)
	#~ return result		
