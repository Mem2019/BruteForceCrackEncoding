#include "BruteForce.h"
#include <exception>
#include <iostream>
#include <memory>
using namespace std;
class BruteForceException : public exception
{
public:
	BruteForceException(const char* info)
	: info(info){}//fucking todo
private:
	const char* info;
};



BruteForce::BruteForce(size_t inputLen, 
	const bfbyte* answer, size_t answerLen, size_t blockSize)
{
	this->answerLen = answerLen;
	
	this->answer = new bfbyte[answerLen + 1];
	memcpy(this->answer, answer, answerLen);
	this->answer[answerLen] = 0;

	this->inputLen = inputLen;
	this->input = new bfbyte[inputLen + 1];
	memset(this->input, 0, sizeof(bfbyte) * (inputLen + 1));
	
	this->inputProg = 0;
	this->keyProg = 0;
	
	initPossibleChars();
	this->numOfNextByteToTrav = 1;

	this->travProgresses = new size_t[inputLen];
	this->newPossibleChars = new bool
		[inputLen][NUM_OF_POSSIBLE_CHARS];

	this->blockSize = blockSize;
}

BruteForce::~BruteForce()
{
	delete[] this->answer;
	delete[] this->input;

	delete[] this->possibleChars;
	delete[] this->numOfPossibleChars;

	delete[] this->travProgresses;
	delete[] this->newPossibleChars;
}

bool BruteForce::traverseNext()
{
	bool isFinish = true;
	for (size_t i = 0; i < numOfNextByteToTrav; i++)
	{
		input[inputProg + i] = possibleChars[inputProg + i][travProgresses[i]];
	}//appply the current char sets
	for (size_t i = 0; i < numOfNextByteToTrav; i++)
	{
#ifdef _DEBUG
		if (numOfPossibleChars[inputProg + i] == 0)
		{
			throw BruteForceException("unexpected error");
		}
#endif // DEBUG

		if (travProgresses[i] == numOfPossibleChars[inputProg + i] - 1)
		{
			travProgresses[i] = 0;
		}
		else
		{
			travProgresses[i]++;
			isFinish = false;
			break;
		}
	}//traverse to next char set
	return isFinish;
}
size_t BruteForce::substrLen(const bfbyte* x)
{
	size_t i;
	for (i = 0; i < answerLen; i++)
	{
		if (x[i] != this->answer[i])
			return i;
	}
	return i;
}

void BruteForce::initPossibleChars()
{
	this->possibleChars = new bfbyte
		[inputLen][NUM_OF_POSSIBLE_CHARS];
	for (size_t i = 0; i < inputLen; i++)
	{
		for (bfbyte c = 0; c < MAX_VAL_OF_BFBYTE; c++)
		{
			this->possibleChars[i][c] = c;
		}
		this->possibleChars[i][MAX_VAL_OF_BFBYTE] = MAX_VAL_OF_BFBYTE;
	}

	this->numOfPossibleChars = new size_t[inputLen];
	for (size_t i = 0; i < inputLen; i++)
	{
		this->numOfPossibleChars[i] = NUM_OF_POSSIBLE_CHARS;
	}
}

void BruteForce::resetNewPossibleChars()
{
	for (size_t i = 0; i < numOfNextByteToTrav; i++)
	{
		for (size_t j = 0; j < NUM_OF_POSSIBLE_CHARS; j++)
		{
			newPossibleChars[i][j] = false;
		}
		newPossibleChars[i][input[inputProg + i]] = true;
	}
}

bool BruteForce::testEncodeResult(const bfbyte * answer)
{//todo, possible out of index access

	size_t prefLen = substrLen(answer);

	if (prefLen >= keyProg + blockSize)
	{
		//if the prefix length reach the next block size, reset
		resetNewPossibleChars();
		keyProg = prefLen / blockSize * blockSize;
		//make keyProg to be the multiple of blockSize
	}
	else if (prefLen >= keyProg)
	{
		//if doesn't reach the new block, 
		//but still reach the current progress, 
		//we record
		for (size_t i = 0; i < numOfNextByteToTrav; i++)
		{
			newPossibleChars[i][input[inputProg + i]] = true;
		}
 	}
 	else
 	{
		//if doesn't reach current progress, 
		//we don't recode
		//int bp = 1; do nothing
 	}
	return (prefLen >= answerLen);//return true if reach the destination
}

void BruteForce::startCrack()
{
	while (true)
	{
		for (size_t i = 0; i < numOfNextByteToTrav; i++)
		{
			travProgresses[i] = 0;
			for (size_t j = 0; j < NUM_OF_POSSIBLE_CHARS; j++)
			{
				newPossibleChars[i][j] = false;
			}
		}
		//init the progress the new possible chars
		while (true)
		{
			bool ret = traverseNext();
			doEncode();//must call testEncodeResult in some way
			if (ret)
				break;
		}
		//update the possible chars from new possible chars
		size_t inputProgAdd = 0;
		size_t numOfNewFixBytes = 0;
		bool isStillSingle = true;
		for (size_t i = 0; i < numOfNextByteToTrav; i++)
		{
			numOfPossibleChars[inputProg + i] = 0;
			bfbyte lastChar;
			for (size_t c = 0; c <= MAX_VAL_OF_BFBYTE; c++)
			{
				//update possible chars from new possible chars
				if (newPossibleChars[i][c])
				{
					lastChar = (bfbyte)c;
					possibleChars[inputProg + i][
						numOfPossibleChars[inputProg + i]] = (bfbyte)c;
					numOfPossibleChars[inputProg + i]++;
				}
			}
			if (numOfPossibleChars[inputProg + i] == 0)
			{
				throw BruteForceException("we don't support blocked algorithm yet");
			}
			else if (numOfPossibleChars[inputProg + i] == 1)
			{
				if (isStillSingle)
				{//if continues to be single, increase the progress
					input[inputProg + i] = lastChar;
					inputProgAdd++;
					numOfNewFixBytes++;
				}
			}
			else
			{
				isStillSingle = false;
			}
		}
		inputProg += inputProgAdd;
		numOfNextByteToTrav -= numOfNewFixBytes;
		numOfNextByteToTrav++;//traverse next byte
		if (numOfNextByteToTrav + inputProg > inputLen)
		{//if the byte is out of index, the crack is finished
			break;
		}
	}

}

size_t BruteForce::getInput(bfbyte * dst, size_t dstlen)
{
	if (dstlen < inputLen)
	{
		throw BruteForceException("dst len not enough");
	}
	return getInput(dst);
}

size_t BruteForce::getInput(bfbyte * dst)
{//need to prevent buffer overflow by yourself!
	memcpy(dst, input, inputLen);
	return inputProg + numOfNextByteToTrav;
}
