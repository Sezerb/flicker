#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define ARRAY_SIZE 9
#define LOW_REG_LIMIT 0
#define HIGH_REG_LIMIT 255

void setVcom(int iReg);
void waitForPanel(int ms);
int getFlicker(void);
int flickerSolution();

int globalIdx = 0;
//Sample Flicker values for different Vcom Register Values.{0, 32, 64, 96, 128, 160, 192, 224, 255}
int A[ARRAY_SIZE] = {84, 111, 119, 117, 96, 45, 33, 86, 114};
//int A[ARRAY_SIZE] = {114, 86, 33, 45, 96, 117, 119, 111, 84};
//int A[ARRAY_SIZE] = {72, 60, 48, 36, 22 ,10, 10, 20, 30}; 
//int A[ARRAY_SIZE] = {50, 35, 22, 10, 4, 15, 30, 42, 54};


int main(int argc, char* argv[])
{
	printf("\nRegVal:%u",flickerSolution());
	return 0;
}

int flickerSolution()
{
	int i;
	int iArrFlickerVal[3]; //0:Low; 1: Center; 2:High
	int iArrRegVal[3]; //0:Low; 1:Center; 2:High
	int iLocalMinima;
	int iLeftCentFlic, iRightCentFlic;
	
	iArrRegVal[0] = LOW_REG_LIMIT;
	iArrRegVal[1] = (LOW_REG_LIMIT+HIGH_REG_LIMIT)/2;
	iArrRegVal[2] = HIGH_REG_LIMIT;


	//Find boundary and center flicker values.
	for(i = 0; i < 3; i++)
	{
		setVcom(iArrRegVal[i]);
		waitForPanel(200);
		iArrFlickerVal[i] = getFlicker();	
		printf("\nBefore Reg:%d Flic:%d", iArrRegVal[i], iArrFlickerVal[i]);			
	}

	//Measure Left and Right Centers
	//1.Left:
	setVcom((iArrRegVal[0] + iArrRegVal[1])/2);
	waitForPanel(200);
	iLeftCentFlic = getFlicker();
	//2.Right:
	setVcom((iArrRegVal[1] + iArrRegVal[2])/2);
	waitForPanel(200);
	iRightCentFlic = getFlicker();

	printf("\nLCReg:%d LCFlic:%d", (iArrRegVal[0] + iArrRegVal[1])/2, iLeftCentFlic);
	printf("\nRCReg:%d RCFlic:%d", (iArrRegVal[1] + iArrRegVal[2])/2, iRightCentFlic);
	
	if(iLeftCentFlic < iRightCentFlic)
	{
		iArrRegVal[2] = iArrRegVal[1];
		iArrRegVal[1] = (iArrRegVal[0] + iArrRegVal[2])/2;
		iArrFlickerVal[2] = iArrFlickerVal[1];
		iArrFlickerVal[1] = iLeftCentFlic;	
	}
	else
	{
		iArrRegVal[0] = iArrRegVal[1];
		iArrRegVal[1] = (iArrRegVal[0] + iArrRegVal[2])/2;
		iArrFlickerVal[0] = iArrFlickerVal[1];
		iArrFlickerVal[1] = iRightCentFlic;		
	}	

	for(i = 0; i < 3; i++)
	{	
		printf("\nAfter Reg:%d Flic:%d", iArrRegVal[i], iArrFlickerVal[i]);			
	}

	while(abs(iArrRegVal[2] - iArrRegVal[0]) > 8)
	{	

		//Find the side of local minima: left of center OR right of center?
		if(abs(iArrFlickerVal[0] - iArrFlickerVal[1]) < abs(iArrFlickerVal[1] - iArrFlickerVal[2]))
		{
			//local minima at the left of center
			iArrRegVal[2] = iArrRegVal[1];
			iArrFlickerVal[2] = iArrFlickerVal[1];
		}
		else
		{
			//local minima at the right of center
			iArrRegVal[0] = iArrRegVal[1];
			iArrFlickerVal[0] = iArrFlickerVal[1];		
		}

		iArrRegVal[1] = (iArrRegVal[0] + iArrRegVal[2])/2;
		setVcom(iArrRegVal[1]);
		waitForPanel(200);
		iArrFlickerVal[1] = getFlicker();
	}	
	
	return iArrRegVal[1];
}

void setVcom(int iReg)
{
	globalIdx = (iReg/31);
}

int getFlicker(void)
{
	if(globalIdx < ARRAY_SIZE)
		return A[globalIdx];
	else
		return 0;
}
void waitForPanel(int ms)
{
	//printf("\nWaitForPanel:%d\n", ms);
}