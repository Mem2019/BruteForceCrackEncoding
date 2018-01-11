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


template<typename F>
class CrackCtf : public BruteForce
{
public:
	CrackCtf(size_t inputLen, const bfbyte* answer, size_t answerLen, F doEnc)
		:BruteForce(inputLen, answer, answerLen), doEnc(doEnc) {}
	CrackCtf(size_t inputLen, const bfbyte* answer, size_t answerLen)
		:BruteForce(inputLen, answer, answerLen) {}
	void setDoEnc(F func)
	{
		doEnc = func;
	}
	~CrackCtf() {};
private:
	F doEnc;
	virtual void doEncode() override
	{//继承重写doEncode函数，他必须要通过getInput获取到当前输入，然后把这个输入加密，再把加密结果作为参数调用testEncodeResult
	 //this function must call getInput to get the input, encode it, 
	 //and call testEncodeResult with the result of encoding
		doEnc();
	}
};