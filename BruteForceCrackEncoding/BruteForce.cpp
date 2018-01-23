#include "BruteForce.h"
#include <exception>
#include <iostream>
#include <memory>
using namespace std;
class BruteForceException : public exception
{
public:
	BruteForceException(const char* info)
	: info(info){}//todo
private:
	const char* info;
};



BruteForce::BruteForce(size_t inputLen, 
	const bfbyte* answer, size_t answerLen, 
	size_t blockSize,
	bfbyte* charset, size_t charsetLen)
{
	this->answerLen = answerLen;
	
	this->answer = new bfbyte[answerLen + 1];
	memcpy(this->answer, answer, answerLen);
	this->answer[answerLen] = 0;

	this->inputLen = inputLen;
	this->input = new bfbyte[inputLen + 1];
	memset(this->input, 0, sizeof(bfbyte) * (inputLen + 1));
	
	this->inputProg = 0;
	this->lockedPrefixNum = 0;
	
	if (charset == nullptr)
	{
		initPossibleChars();
	}
	else
	{
		initPossibleChars(charset, charsetLen);
	}
	this->numOfNextByteToTrav = 1;

	this->travProgresses = new size_t[inputLen];
	this->newPossibleChars = new bool
		[inputLen][NUM_OF_POSSIBLE_CHARS];

	this->blockSize = blockSize;

	this->lockedProgForEachInput = new size_t[inputLen];
}

BruteForce::~BruteForce()
{
	delete[] this->answer;
	delete[] this->input;

	delete[] this->possibleChars;
	delete[] this->numOfPossibleChars;

	delete[] this->travProgresses;
	delete[] this->newPossibleChars;

	delete[] this->lockedProgForEachInput;
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

void BruteForce::initPossibleChars(bfbyte * charset, size_t charsetLen)
{
	this->possibleChars = new bfbyte
		[inputLen][NUM_OF_POSSIBLE_CHARS];
	for (size_t i = 0; i < inputLen; i++)
	{
		for (size_t c = 0; c < charsetLen && c < NUM_OF_POSSIBLE_CHARS; c++)
		{
			this->possibleChars[i][c] = charset[c];
		}
	}

	this->numOfPossibleChars = new size_t[inputLen];
	for (size_t i = 0; i < inputLen; i++)
	{
		this->numOfPossibleChars[i] = charsetLen;
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

	if (prefLen >= lockedPrefixNum + blockSize)
	{
		//if the prefix length reach the next block size, reset
		resetNewPossibleChars();
		lockedPrefixNum = prefLen / blockSize * blockSize;
		//make keyProg to be the multiple of blockSize
	}
	else if (prefLen >= lockedPrefixNum)
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
		//we don't record
		//However, if prefLen retrogrades, throw exception!
		//This is not accurate, still..
		if (inputProg > 0 && prefLen < lockedProgForEachInput[inputProg - 1])
		{
			int asd = 0;
			//throw BruteForceException("The block size may be wrong!");
		}
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
				//this detection is not accurate.
				//since it is possible for 
				//the next byte to reach the previous progress,
				//but actually with combination of these byte into one block
				//the progress can be even higher
				//e.g. Zctf20171, encode in block of 2 bytes
				//will stuck in first two bytes, with keyProgress 1
				//since these two bytes are guessed wrong
				//actually the progress can be 2 
				//if we traverse all of 65536 cases
				throw BruteForceException("The block size may be wrong!");
			}
			else if (numOfPossibleChars[inputProg + i] == 1)
			{
				if (isStillSingle)
				{//if continues to be single, increase the progress
					input[inputProg + i] = lastChar;
					numOfNewFixBytes++;
					lockedProgForEachInput[inputProg + i]
						= lockedPrefixNum;
					if (lockedPrefixNum == 0x0A)//debug
					{
						int asd = 0;
					}
				}
			}
			else
			{
				isStillSingle = false;
			}
		}
		inputProg += numOfNewFixBytes;
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
