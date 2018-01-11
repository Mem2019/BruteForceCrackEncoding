#pragma once
typedef unsigned char bfbyte;
#define NUM_OF_POSSIBLE_CHARS 256
#define MAX_VAL_OF_BFBYTE 255
class BruteForce
{
private:
	size_t substrLen(const bfbyte* x);
	void initPossibleChars();
	void resetNewPossibleChars();

	bfbyte* input;
	size_t inputLen;
	size_t inputProg;

	bfbyte* answer;
	size_t answerLen;

	bfbyte (*possibleChars)[NUM_OF_POSSIBLE_CHARS];
	size_t* numOfPossibleChars;
	//number of possible chars for each input
	//will decrease as the crack does

	//number of next byte to traverse
	size_t numOfNextByteToTrav;
	//traverse progresses, with the length of numOfNextByteToTrav
	//which means which is the next index of possible char to fetch
	size_t* travProgresses;
	//the new possible chars to record, with the length of numOfNextByteToTrav
	bool (*newPossibleChars)[NUM_OF_POSSIBLE_CHARS];

	size_t keyProg;
protected:
	bool traverseNext();
	virtual void doEncode() = 0;
	//do in encode must call testEncodeResult 
	//with encoded result of current input
	//(obtained by getInput) in some way
public:
	BruteForce(size_t inputLen, const bfbyte* answer, size_t answerLen);
	virtual ~BruteForce();

	void startCrack();
	size_t getInput(bfbyte* dst, size_t dstlen);
	bool testEncodeResult(const bfbyte* answer);
	size_t getInput(bfbyte * dst);
};

