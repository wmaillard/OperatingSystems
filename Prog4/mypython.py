# William Maillard
# Program 5: mypython.py
# CS344
# 5/30/16

#This program writes three random strings of lowercase integers to three different files
#The filenames are 'File1.txt', 'File2.txt', and 'File3.txt'
#If those files already exist, '(copy)' will be appended to their filename
#This program will output the contents of the files and the files' names
#After writing the files, this program will output two random integers between 1 and 42, inclusive
#It will then output the product of these two integers 

import os.path
import string
import random


def main():
	
	print '\n'
	
	files = []

	for i in range(1, 4):
		fileName = "File" + str(i)
		while(os.path.isfile(fileName + '.txt')):
			fileName = fileName + '(copy)'
		fileName = fileName + '.txt'
		files.append(fileName)
		
	theLetters = []	
	for j in range(3):
		randomLetters = ""
		for i in range(10):
			randomLetters = randomLetters + random.choice(string.ascii_lowercase)
		theLetters.append(randomLetters)
	
	i = 0
	for fileNames in files:
		with open(fileNames, 'wb') as newFile:
			print 'Writing "' + theLetters[i] + '" plus a newline to file: ' + fileNames + "\n"
			newFile.write(theLetters[i] + '\n')
		i = i + 1
		newFile.close()
		print 'Done\n'
		
	from random import randint
	one = randint(1, 42)
	print 'Random integer one: ' + str(one) + '\n'
	two = randint(1, 42)
	print 'Random integer two: ' + str(two) + '\n'
	theProduct = one * two
	print 'The product of these integers is: ' + str(theProduct) + '\n'
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	

if __name__ == '__main__':
	main()