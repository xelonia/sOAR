/**
* \file UtNArray.cpp
* \brief Implementation of UtNArray to test the NArray class
*
* (C) Karsten Isakovic, Berlin 2016 ( Karsten.Isakovic@web.de )
*/
#include <math.h>
#include <stdlib.h>

#include "UnitTest.h"

#include "../soar_support_lib/NArray.h"


/**
 * \ingroup Tests
 * \brief UnitTest for NArray class
 */
class UtNArray : public UnitTest {
private:
	double _eps;				///< Epsilon used for comparison

public:

	/// \brief The constructor registers at the UnitTestManager
	UtNArray() : UnitTest("NArray") , _eps(0.0000001)
	{
	}

	template<typename T> void GenNArray(NArray<T> &arr, int &idx, int dims, int dim[10]) {
		for(int i=0;i<10;i++) {
			int w = GetNextIntTestValue(idx, 1,7);
			dim[i] = (i<dims) ? w : 0;
		}
//		fprintf(stderr,"----NArray %2d DIM(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)\n",
//				dims, dim[0],dim[1],dim[2],dim[3],dim[4],dim[5],dim[6],dim[7],dim[8],dim[9]);
		if (dims!=0)
			arr.Init(dim[0],dim[1],dim[2],dim[3],dim[4],dim[5],dim[6],dim[7],dim[8],dim[9]);
	}

	template<typename T> T & AccessNArray(NArray<T> &arr, int dims, unsigned int d0, unsigned int d1, unsigned int d2, 
			unsigned int d3, unsigned int d4, unsigned int d5, unsigned int d6, 
			unsigned int d7, unsigned int d8, unsigned int d9) 
	{
//		fprintf(stderr,"AccessNArray %2d DIM(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)\n",	dims, d0,d1,d2,d3,d4,d5,d6,d7,d8,d9);						
		switch(dims) 
		{
		case 1: return arr(d0); 
		case 2: return arr(d0,d1);
		case 3: return arr(d0,d1,d2);
		case 4: return arr(d0,d1,d2,d3);
		case 5: return arr(d0,d1,d2,d3,d4);
		case 6: return arr(d0,d1,d2,d3,d4,d5);
		case 7: return arr(d0,d1,d2,d3,d4,d5,d6);
		case 8: return arr(d0,d1,d2,d3,d4,d5,d6,d7);
		case 9: return arr(d0,d1,d2,d3,d4,d5,d6,d7,d8);
		case 10:return arr(d0,d1,d2,d3,d4,d5,d6,d7,d8,d9);
		default: 
			fprintf(stderr,"ERROR in UnitTest_NArray::ReadNArray(dims=%d)\n",dims);
			throw 42;
		}
	}

	void GenAccessInBound(int &idx, int dims, int dim[10],int acc[10]) {
		for(int i=0; i<10;i++) {
			if (i<dims)
				acc[i] = GetNextIntTestValue(idx, 0, dim[i]-1);
			else 
				acc[i] = 0;
		}
	}

	int Func10Dim(unsigned int d0, unsigned int d1, unsigned int d2, 
			unsigned int d3, unsigned int d4, unsigned int d5, unsigned int d6, 
			unsigned int d7, unsigned int d8, unsigned int d9) 
	{
		d0 += 1;
		d1 += 2;
		d2 += 3;
		d3 += 4;
		d4 += 5;
		d5 += 6;
		d6 += 7;
		d7 += 8;
		d8 += 9;
		d9 += 10;
		return d0*d1*d2*d3*d4*d5*d6*d7*d8*d9;
	}



	void TestArrayInitialization() {
		TestGroup("Array fill");
		//=============================== Test Fill()   (N-test cases)

		int idx0 = 31; 	
		for(int dims=1; dims<=10; dims++) {
			for(int tests=0; tests<5; tests++) {

				// Construct 1D..10D array
				int         dim[10];
				NArray<int> arr;

				GenNArray<int>(arr, idx0, dims, dim);

				int fillVal = GetNextIntTestValue(idx0, -1000, 1000);
				arr.Fill( fillVal );

				int totalSize = arr.GetSize();
				int *data     = arr.GetData();

				bool same=true;
				for(int t=0; t<totalSize; t++) {
					if (data[t] != fillVal) {
						same = false;
						break;
					}
				}
				ExpectOkay(same, "Fill value differs");
			}
		}

		TestGroup("Array initial values");
		//=============================== Test values after Init() for false or zero of different Types (N-test cases)
		{
			int arrSize = 20;

			NArray<bool>           bArray(arrSize);
			NArray<char>           cArray(arrSize);
			NArray<unsigned char>  ucArray(arrSize);
			NArray<short>          sArray(arrSize);
			NArray<unsigned short> usArray(arrSize);
			NArray<int>			   iArray(arrSize);
			NArray<unsigned int>   uiArray(arrSize);
			NArray<float>		   fArray(arrSize);
			NArray<double>		   dArray(arrSize);

			for(int i=0; i<arrSize; i++) {
				ExpectOkay( bArray[i] == false, "NArray<bool> init value");
				ExpectOkay( cArray[i] == 0,   "NArray<char> init value");
				ExpectOkay(ucArray[i] == 0,   "NArray<unsigned char> init value");
				ExpectOkay( sArray[i] == 0,   "NArray<short> init value");
				ExpectOkay(usArray[i] == 0,   "NArray<unsigned short> init value");
				ExpectOkay( iArray[i] == 0,   "NArray<int> init value");
				ExpectOkay(uiArray[i] == 0,   "NArray<unsigned int> init value");
				ExpectOkay( fArray[i] == 0.0, "NArray<float> init value");
				ExpectOkay( dArray[i] == 0.0, "NArray<double> init value");
			}
		}

		
		TestGroup("Array Initialization");
		//=============================== Zero dimensional array
		bool assertThrown = false;	
		try {
			NArray<bool> dim0;
			dim0.Init(0); // Since zero marks the end of Init() parameter list, this is zero dimensions
		}
		catch( int e) {
			e = e;
			assertThrown = true;
		}
		ExpectOkay(assertThrown, "Zero dimensional array");


		//=============================== Zero in wrong position at Init()
		for(int err=0; err<9; err++) {
			// Construct 10D array, but one dimension width is zero.
			int dim[10];
			for(int i=0;i<10;i++) {
				dim[i] = (i==err) ? 0 : 2;
			}

			bool assertThrown = false;	
			try {
				NArray<int> arr;
				arr.Init(dim[0],dim[1],dim[2],dim[3],dim[4],dim[5],dim[6],dim[7],dim[8],dim[9]);
			}
			catch( int e) {
				e = e;
				assertThrown = true;
			}
			ExpectOkay(assertThrown, "Zero in wrong position %d at Init()",err);
		}

		//=============================== Copy constructor
		int idx1 = 31;
		for(int dims=0; dims<=10; dims++) {
			for(int tests=0; tests<5; tests++) {

				// Construct 0D..10D array
				int         dim[10];
				int         acc[10];
				NArray<int> arrA;

				GenNArray<int>(arrA, idx0, dims, dim);
				
				if (dims!=0) {
					// Fill with random values
					for(unsigned int v=0; v<100; v++) {				
						GenAccessInBound(idx1,dims,dim,acc);

						AccessNArray<int>(arrA, dims, acc[0],acc[1],acc[2],acc[3],acc[4],acc[5],acc[6],acc[7],acc[8],acc[9]) = 
							GetNextIntTestValue(idx1, -1000, 1000);
					}
				}

				// Construct other array using copy constructor
				NArray<int> arrB( arrA );
				
				ExpectOkay(arrA==arrB, "Copy constructor");
			}
		}
		TestGroup();
	}




	void TestInBoundsAccess() {
		TestGroup("In-bounds     access 1-10D");

		int maxTries = 25;
		int idx0 = 100; 	
		for(int dims=1; dims<=10; dims++) {
			for(int tests=0; tests<5; tests++) {

				// Construct 1D..10D array
				int         dim[10];
				int         acc[10];
				NArray<int> arr;

				GenNArray<int>(arr, idx0, dims, dim);

				// Write N values to the current array
				int idx1 = 100;
				for(int tries=0; tries<maxTries; tries++) {

					bool assertThrown = false;	
					try {
						GenAccessInBound(idx1,dims,dim,acc);

						int func = Func10Dim(acc[0],acc[1],acc[2],acc[3],acc[4],acc[5],acc[6],acc[7],acc[8],acc[9]);

						AccessNArray<int>(arr, dims, acc[0],acc[1],acc[2],acc[3],acc[4],acc[5],acc[6],acc[7],acc[8],acc[9]) = func;
					}
					catch( int e) {
						e = e;
						assertThrown = true;
					}
					ExpectOkay(!assertThrown, "%dD array write access inside bounds",dims);
				}

				// Read N values to the current array and compare with expected values
				idx1 = 100;
				for(int tries=0; tries<maxTries; tries++) {

					try {
						GenAccessInBound(idx1,dims,dim,acc);

						int exptVal = Func10Dim(acc[0],acc[1],acc[2],acc[3],acc[4],acc[5],acc[6],acc[7],acc[8],acc[9]);
						int readVal = AccessNArray<int>(arr, dims, acc[0],acc[1],acc[2],acc[3],acc[4],acc[5],acc[6],acc[7],acc[8],acc[9]);
						bool result = exptVal==readVal;

						ExpectOkay(result, "%dD array read access inside bounds differs",dims);

						if (!result)
							break;
					}
					catch( int e) {
						e = e;
						ExpectOkay(false, "%dD array read access inside bounds",dims);
					}

				}

			}
		}
		TestGroup();
	}




	void TestOutOfBoundsAccess() {
		TestGroup("Out-of-bounds access 1-10D");

		int idx0 = 200; 	
		for(int dims=1; dims<=10; dims++) {
			for(int tests=0; tests<5; tests++) {

				// Construct 1D..10D array
				int dim[10];
				NArray<int> arr;
				GenNArray<int>(arr, idx0, dims, dim);				

				// Write N values to the current array
				int idx1 = 100;
				for(int tries=0; tries<50; tries++) {
					bool willThrow    = true;
					bool assertThrown = false;	
					try {
						// Access out of bounds				
						int acc[10];
						for(int i=0; i<10;i++) {
							if (i<dims)
								acc[i] = GetNextIntTestValue(idx1, dim[i], dim[i]*10);
							else 
								acc[i] = 0;
						}

						int val = AccessNArray<int>(arr, dims, acc[0],acc[1],acc[2],acc[3],acc[4],acc[5],acc[6],acc[7],acc[8],acc[9]);
					}
					catch( int e) {
						e = e;
						assertThrown = true;
					}
					ExpectOkay(willThrow==assertThrown, "%dD array access out of bounds",dims);
				}
			}
		}
		TestGroup();
	}




	void TestCompareOperator() {
		TestGroup("Compare operator     0-10D");

		//=============================== Compare operator with identical data (N-test cases)
		int idx0 = 100; 	
		for(int dims=1; dims<=10; dims++) {
			for(int tests=0; tests<50; tests++) {

				// Construct 1D..10D array
				int dim[10];
				int acc[10];
				NArray<int> arrA;
				NArray<int> arrB;
				
				GenNArray<int>(arrA, idx0, dims, dim);
				arrB.Init(dim[0],dim[1],dim[2],dim[3],dim[4],dim[5],dim[6],dim[7],dim[8],dim[9]);

				// Write N values to both arrays
				int idx1 = 175;
				for(int tries=0; tries<1000; tries++) {

					try {
						GenAccessInBound(idx1,dims,dim,acc);

						int func = Func10Dim(acc[0],acc[1],acc[2],acc[3],acc[4],acc[5],acc[6],acc[7],acc[8],acc[9]);

						AccessNArray<int>(arrA, dims, acc[0],acc[1],acc[2],acc[3],acc[4],acc[5],acc[6],acc[7],acc[8],acc[9]) = func;
						AccessNArray<int>(arrB, dims, acc[0],acc[1],acc[2],acc[3],acc[4],acc[5],acc[6],acc[7],acc[8],acc[9]) = func;				
					}
					catch( int e) {
						e = e;
						ExpectOkay(false, "%dD array Compare setup",dims);
						break;
					}
				}

				try {
					ExpectOkay(arrA==arrB, "%dD array Compare",dims);
				}
				catch( int e) {
					e = e;
					ExpectOkay(false, "%dD array Compare assertion",dims);
					break;
				}


			}
		}

		//=============================== Compare operator with different data (N-test cases)
		idx0 = 150; 	
		for(int dims=1; dims<=10; dims++) {
			for(int tests=0; tests<50; tests++) {

				// Construct 1D..10D array
				int dim[10], acc[10];
				NArray<int> arrA;
				GenNArray<int>(arrA, idx0, dims, dim);

				// Write N values to first arrays
				int idx1 = 175;
				
				int dif[10];
				for(int tries=0; tries<1000; tries++) {

					try {
						GenAccessInBound(idx1,dims,dim,acc);

						int func = Func10Dim(acc[0],acc[1],acc[2],acc[3],acc[4],acc[5],acc[6],acc[7],acc[8],acc[9]);

						// At one point, sample the location
						if (tries==500) {
							for(int i=0;i<10;i++)
								dif[i] = acc[i];
						}

						AccessNArray<int>(arrA, dims, acc[0],acc[1],acc[2],acc[3],acc[4],acc[5],acc[6],acc[7],acc[8],acc[9]) = func;
					}
					catch( int e) {
						e = e;
						ExpectOkay(false, "%dD array Compare setup with different arrays",dims);
						break;
					}
				}
				
				NArray<int> arrB;
				arrB = arrA; // Identical copy of A

				// Change just 1 position
				AccessNArray<int>(arrB, dims, dif[0],dif[1],dif[2],dif[3],dif[4],dif[5],dif[6],dif[7],dif[8],dif[9]) = -1;				

				try {
					ExpectOkay( !(arrA==arrB), "%dD array Compare with different arrays",dims);
				}
				catch( int e) {
					e = e;
					ExpectOkay(false, "%dD array Compare assertion with different arrays",dims);
					break;
				}
			}
		}

		
		//=============================== Compare operator with different dimension arrays
		idx0 = 150; 	
		int idx1 = 41; 	
		for(int dimsA=0; dimsA<=10; dimsA++) {
			for(int dimsB=0; dimsB<=10; dimsB++) {
				for(int tests=0; tests<10; tests++) {

					// Construct 1D..10D array
					int dimA[10],dimB[10];

					NArray<int> arrA;
					GenNArray<int>(arrA, idx0, dimsA, dimA);	


					NArray<int> arrB;
					GenNArray<int>(arrB, idx1, dimsB, dimB);			

					bool expectSame = dimsA==dimsB;
					if (expectSame) {
						for(int i=0; i<10;i++) {
							if (dimA[i]!=dimB[i]) {
								expectSame = false;
								break;
							}
						}
					}

					try {
						if (expectSame)
							ExpectOkay( arrA==arrB, "%dD array Compare with different array layout %dD",dimsA,dimsB);
						else
							ExpectFail( arrA==arrB, "%dD array Compare with different array layout %dD",dimsA,dimsB);
					}
					catch( int e) {
						e = e;
						ExpectOkay(false, "%dD array Compare with different array layout %dD",dimsA,dimsB);
						break;
					}
				}
			}
		}

		TestGroup();
	}




	void TestArrayAssignment() {
		TestGroup("Array assignment diff dims");
		//=============================== Assignment of different dimensions not allowed for initialzed array

		int idxA = 17;
		int idxB = 41;
		for(int tests=0; tests<250; tests++) {
			int dimsA,dimsB, dimA[10],dimB[10];

			// Construct two 1D..10D arrays
			NArray<int> arrA,arrB;

			bool sameDim;	
			do {
				dimsA = GetNextIntTestValue(idxA, 1, 10);
				dimsB = GetNextIntTestValue(idxB, 1, 10);

				GenNArray<int>(arrA, idxA, dimsA, dimA);
				GenNArray<int>(arrB, idxB, dimsB, dimB);

				sameDim = dimsA==dimsB;
				if (sameDim) {
					for(int i=0;i<10;i++) {
						if (dimA[i]!=dimB[i]) {
							sameDim = false;
							break;
						}
					}
				}
			}
			while(sameDim);

				

			bool assertThrown = false;	
			try {
				arrB = arrA;
				int okay=1;
			}
			catch( int e) {
				e = e;
				assertThrown = true;
			}
			ExpectOkay(assertThrown, "Did not throw assert()");
		}

		TestGroup("Array assignment allowed");
		//=============================== Assignment of different dimensions allowed to uninitialized array
		idxA = 91;
		for(int tests=0; tests<100; tests++) {
			int dimsA = GetNextIntTestValue(idxA, 1, 10);

			// Construct 1D..10D array
			int dimA[10];

			NArray<int> arrA,arrB;
				
			GenNArray<int>(arrA, idxA, dimsA, dimA);

			bool assertThrown = false;	
			try {
				arrB = arrA;
			}
			catch( int e) {
				e = e;
				assertThrown = true;
			}
			ExpectFail(assertThrown, "Did throw assert()");
		}

		TestGroup();
	}




	void Test1DArraySpecials() {
		TestGroup("1D array with const array");
		//=============================== For 1D Array check initialization with constant array
		{
			bool            bConst[] = { true, false, false };
			char            cConst[] = "HelloWorld";
			unsigned char  ucConst[] = { 180, 20, 2, 5, 0 };
			short           sConst[] = { 0, 10000, 32767, -40, -30000 };
			unsigned short usConst[] = { 0, 60000, 100 };
			int             iConst[] = { 1,-2,3,-4,5,-6,7,-8,9,-10 };
			unsigned int   uiConst[] = { 1,2,3,4,5,6,7,8,9,10 };
			float           fConst[] = { 5.0f, 4.0f, 3.5f, 2.2f, 1.1111f };
			double          dConst[] = { 7.45, 3.1415, 8.18576 };

			int  bSize = sizeof( bConst)/sizeof(bool);
			int  cSize = sizeof( cConst)/sizeof(char);
			int ucSize = sizeof(ucConst)/sizeof(unsigned char);
			int  sSize = sizeof( sConst)/sizeof(short);
			int usSize = sizeof(usConst)/sizeof(unsigned short);
			int  iSize = sizeof( iConst)/sizeof(int);
			int uiSize = sizeof(uiConst)/sizeof(unsigned int);
			int  fSize = sizeof( fConst)/sizeof(float);
			int  dSize = sizeof( dConst)/sizeof(double);

			NArray<bool>            bArr( bConst);		
			NArray<char>            cArr( cConst);		
			NArray<unsigned char>  ucArr(ucConst);		
			NArray<short>           sArr( sConst);		
			NArray<unsigned short> usArr(usConst);		
			NArray<int>             iArr( iConst);		
			NArray<unsigned int>   uiArr(uiConst);		
			NArray<float>           fArr( fConst);	
			NArray<double>          dArr( dConst);			

			ExpectOkay( bArr.GetDims()==1 &&  bArr.GetDim(0)== bSize, "NArray<bool> size/dims differs");
			ExpectOkay( cArr.GetDims()==1 &&  cArr.GetDim(0)== cSize, "NArray<char> size/dims differs");
			ExpectOkay(ucArr.GetDims()==1 && ucArr.GetDim(0)==ucSize, "NArray<unsigned char> size/dims differs");
			ExpectOkay( sArr.GetDims()==1 &&  sArr.GetDim(0)== sSize, "NArray<short> size/dims differs");
			ExpectOkay(usArr.GetDims()==1 && usArr.GetDim(0)==usSize, "NArray<unsigned short> size/dims differs");
			ExpectOkay( iArr.GetDims()==1 &&  iArr.GetDim(0)== iSize, "NArray<int> size/dims differs");
			ExpectOkay(uiArr.GetDims()==1 && uiArr.GetDim(0)==uiSize, "NArray<int> size/dims differs");
			ExpectOkay( fArr.GetDims()==1 &&  fArr.GetDim(0)== fSize, "NArray<float> size/dims differs");
			ExpectOkay( dArr.GetDims()==1 &&  dArr.GetDim(0)== dSize, "NArray<double> size/dims differs");

			for(int i=0; i<bSize; i++)
				ExpectOkay( bArr[i] == bConst[i], "NArray<bool> differs");

			for(int i=0; i<cSize; i++)
				ExpectOkay( cArr[i] == cConst[i], "NArray<char> differs");

			for(int i=0; i<ucSize; i++)
				ExpectOkay( ucArr[i] == ucConst[i], "NArray<unsigned char> differs");

			for(int i=0; i<sSize; i++)
				ExpectOkay( sArr[i] == sConst[i], "NArray<short> differs");

			for(int i=0; i<usSize; i++)
				ExpectOkay( usArr[i] == usConst[i], "NArray<unsigned short> differs");

			for(int i=0; i<iSize; i++)
				ExpectOkay( iArr[i] == iConst[i], "NArray<int> differs");

			for(int i=0; i<uiSize; i++)
				ExpectOkay( uiArr[i] == uiConst[i], "NArray<unsigned int> differs");

			for(int i=0; i<fSize; i++)
				ExpectOkay( fArr[i] == fConst[i], "NArray<float> differs");

			for(int i=0; i<dSize; i++)
				ExpectOkay( dArr[i] == dConst[i], "NArray<double> differs");

		}

		TestGroup("1D array specials");
		//=============================== For 1D Array check (x) versus [x]
		int idx0 = 0;
		for(int test=0; test<20; test++) {
			int width = GetNextIntTestValue(idx0, 1,37);

			NArray<int> arr1D(width);

			// Fill Array using (x) operator
			int idx1 = 0;
			for(int x=0; x<width; x++)
				arr1D(x) = (int)GetNextTestValue(idx1);

			// Retrieve array using [x] operator
			idx1 = 0;
			for(int x=0; x<width; x++) {
				int exptVal = (int)GetNextTestValue(idx1);
				int readVal = arr1D[x];
				bool result = exptVal==readVal;
				ExpectOkay(result, "1D array(size=%d) acccess [%d]=%d differs from (%d)=%d",width, x,readVal,x,exptVal);
			}				
		}


		TestGroup();
	}

	void TestBinaryFileIO() {
		TestGroup("Load/Save/Compare Binary");


		// Compare NArrays with random different layouts, but same data
		int idx0 = 150; 	
		int idx1 = 41; 	
		for(int dimsA=0; dimsA<=10; dimsA++) {
			for(int dimsB=0; dimsB<=10; dimsB++) {
				for(int tests=0; tests<2; tests++) {

					// Construct 0D..10D array
					int dimA[10],dimB[10];

					NArray<int> arrA;
					GenNArray<int>(arrA, idx0, dimsA, dimA);	

					NArray<int> arrB;
					GenNArray<int>(arrB, idx1, dimsB, dimB);			

					bool expectSame = dimsA==dimsB;
					if (expectSame) {
						for(int i=0; i<10;i++) {
							if (dimA[i]!=dimB[i]) {
								expectSame = false;
								break;
							}
						}
					}

					FILE *file = 0;

					fopen_s(&file,"Ut_N.dat","wb");
					ExpectOkay(arrA.SaveBinary(file), "Saving NArray");
					fclose(file);

					fopen_s(&file,"Ut_N.dat","rb");
					ExpectOkay(expectSame==arrB.CompareBinary(file),"Compare NArray");
					fclose(file);
				}
			}
		}

		// Load/Save Binary error cases
		{

			// Create empty file
			{
				FILE *file = 0;
				fopen_s(&file,"Ut_N.dat","wb");
				fclose(file);

				NArray<int> arr;
				fopen_s(&file,"Ut_N.dat","rb");
				ExpectFail(arr.LoadBinary(file),"Loading empty file");
				fclose(file);
			}

			// Create too short file (missing dimension data)
			{
				FILE *file = 0;
				fopen_s(&file,"Ut_N.dat","wb");
				int n=2;
				fwrite(&n, sizeof(int), 1, file);
				fclose(file);

				NArray<int> arr;
				fopen_s(&file,"Ut_N.dat","rb");
				ExpectFail(arr.LoadBinary(file),"Loading too short file (missing dimension data)");
				fclose(file);
			}

			// Create too short file (missing data)
			{
				FILE *file = 0;
				fopen_s(&file,"Ut_N.dat","wb");
				int n=1;
				int d0=4;
				int data=0;
				fwrite(&n,    sizeof(int), 1, file);
				fwrite(&d0,   sizeof(int), 1, file);
				fwrite(&data, sizeof(int), 1, file); // Only 1 instead of 1 INTs
				fclose(file);

				NArray<int> arr;
				fopen_s(&file,"Ut_N.dat","rb");
				ExpectFail(arr.LoadBinary(file),"Loading too short file (missing data)");
				fclose(file);
			}
			
			// Create too bad file (wrong dimension)
			{
				FILE *file = 0;
				fopen_s(&file,"Ut_N.dat","wb");
				int n=1;
				int d0=123400000;
				int data=4;
				fwrite(&n, sizeof(int), 1, file);
				fwrite(&d0,   sizeof(int), 1, file);
				fwrite(&data, sizeof(int), 1, file); // Only 1 instead of 1 INTs
				fclose(file);

				NArray<int> arr;
				fopen_s(&file,"Ut_N.dat","rb");
				ExpectFail(arr.LoadBinary(file),"Loading bad file (bad dimension)");
				fclose(file);
			}

			// Save binary error
			{
				FILE *file = 0;
				fopen_s(&file,"Ut_N.dat","rb"); // Open for read, but try to write...
				
				NArray<int> arr;
				ExpectFail(arr.SaveBinary(file),"Saving binary on read-only file");
				fclose(file);
			}


		}
		TestGroup();
	}

	void RunTests() {
		TestArrayInitialization();
		TestArrayAssignment();
		TestInBoundsAccess();
		TestOutOfBoundsAccess();
		TestCompareOperator();
		Test1DArraySpecials();
		TestBinaryFileIO();
	}
};

UtNArray test_NArray;  ///< Global instance automatically registers to UnitTestManager
