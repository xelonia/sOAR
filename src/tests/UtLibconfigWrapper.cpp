/**
* \file UtLibconfigWrapper.cpp
* \brief Implementation of UtLibconfigWrapper to test the LibconfigWrapper class
*
* (C) Karsten Isakovic, Berlin 2016 ( Karsten.Isakovic@web.de )
*/

#include <stdio.h>
#include <math.h>
#include "UnitTest.h"

#include "..\soar_support_lib\ParamManager.h"
#include "..\soar_support_lib\ParamManager\LibconfigWrapper.h"
#include "..\soar_support_lib\ParamManager\ParBase.h"
#include "..\soar_support_lib\NArray.h"
#include "..\soar_support_lib\FuncType.h"

/**
* \ingroup Tests
* \brief UnitTest for LibconfigWrapper class
*/
class UtLibconfigWrapper : public UnitTest {
private:
	FILE *_file;

	char  _fileName[512];
	char   _fileContent[2048];

	bool WriteConfigFile(char *str) {
		fopen_s(&_file, _fileName,"w");
		if (_file) {
			sprintf_s(_fileContent,"tmp = %s;",str);
			fprintf(_file,_fileContent);
			fprintf(_file,"\n");
			fclose(_file);	
			return true;
		}
		return false;
	}


	double  ClippedVal(LcwBaseType type, double val) {
		if (type==T_BOOL) 		 return val>5?1:0;
		else if (type==T_CHAR)	 return ((char)val);
		else if (type==T_UCHAR)	 return ((unsigned char)val);
		else if (type==T_SHORT)	 return ((short)val);
		else if (type==T_USHORT) return ((unsigned short)val);
		else if (type==T_INT)	 return (int)val;
		else if (type==T_UINT)	 return ((unsigned int)val);
		else return val;
	}

	bool WriteBuildin(LcwBaseType type, double clippedVal) {
		char tmpStr[512];

		switch(type) {
			case T_BOOL:	sprintf_s(tmpStr,"%s", clippedVal==1?"true":"false"); break;
			case T_CHAR:	sprintf_s(tmpStr,"%d", (int)            clippedVal); break;
			case T_UCHAR:	sprintf_s(tmpStr,"%u", (unsigned int)   clippedVal); break;
			case T_SHORT:	sprintf_s(tmpStr,"%d", (int)            clippedVal); break;
			case T_USHORT:	sprintf_s(tmpStr,"%u", (unsigned int)   clippedVal); break;
			case T_INT:		sprintf_s(tmpStr,"%d", (int)            clippedVal); break;
			case T_UINT:	sprintf_s(tmpStr,"%u", (unsigned int)   clippedVal); break;
			case T_FLOAT:	sprintf_s(tmpStr,"%f", (float)          clippedVal); break;
			case T_DOUBLE:	sprintf_s(tmpStr,"%f",                  clippedVal); break;
			default:        sprintf_s(tmpStr,"<unsupported buildin %d>",type); break;
		}
		return WriteConfigFile(tmpStr);
	}

	bool GetBuildinCompatibility(int tWrite, int tRead, double clippedVal, int rMin, int rMax) {
		bool expectResult = tWrite==tRead;


		int  intVal  = (int)clippedVal;

		bool intType = !(tWrite==T_BOOL || tWrite==T_FLOAT || tWrite==T_DOUBLE);					

		if (tWrite!=tRead) {
			switch(tRead) {
			case T_BOOL:
			case T_CHAR:
			case T_UCHAR:
			case T_SHORT:
			case T_USHORT:
				{
					if ( intType && (intVal>=rMin && intVal<=rMax) )
						expectResult = true;
				}
				break;
			case T_UINT:
				{
					if (tWrite==T_UCHAR || tWrite==T_USHORT)
						expectResult = true;
					else if ( intVal>=0 && intType )
						expectResult = true;
				}
				break;

			case T_INT:
				{
					if (intType) 
						expectResult = true;
				}
				break;

			case T_FLOAT:
			case T_DOUBLE:
				{
					if (tWrite!=T_BOOL)
						expectResult = true;
				}
				break;
			}
		}
		return expectResult;
	}

	bool GenCSV(char *fileName, int & idx, bool idxErr, bool parErr, bool headErr, bool sepComma) 
	{
		char sep;
		FILE *csvFile;

		sep = sepComma?',':';';

		fopen_s(&csvFile, fileName,"w");
		if (!csvFile) {
			printf("GenCSV: Could not open '%s#' for writing\n",fileName);
			return false;
		}

		int w = GetNextIntTestValue(idx,1,10);
		int h = GetNextIntTestValue(idx,4,25);

		if (!headErr) {
			fprintf(csvFile,"Week");
			for(int x=0; x<w;x++) {
				fprintf(csvFile,"%c Loc_%d",sep,x);
			}
			fprintf(csvFile,"\n");			
		}


		int errLine = GetNextIntTestValue(idx,0,h-1);

		for(int y=0; y<h; y++) {
			if (idxErr && y==errLine) {
				fprintf(csvFile,"%d",y+5,sep); // Wrong line number...
			}
			else {
				fprintf(csvFile,"%d",y+1,sep); 
			}

			for(int x=0; x<w; x++) {
				float val = (float)x*0.1f + (float)y;
				if (parErr && y==errLine && x==0) {
					continue; // Skip one parameter
				}
				fprintf(csvFile,"%c %f",sep,val); 				
			}
			fprintf(csvFile,"\n");
		}
		
		fclose(csvFile);	
		return true;
	}

	bool WriteConfigWithCsv(char *name, int testId, int &idx, bool idxErr,bool parErr, bool headErr, bool sepErr, bool unkNameErr, bool noNameErr)
	{
		char csvName[256],cfgName[256];

		sprintf_s(csvName,"Ut_LcW_2dCSV.csv");
		sprintf_s(cfgName,"\"%s\"",csvName);
		if (unkNameErr) 
			sprintf_s(cfgName,"\"unknown.csv\"");
		if (noNameErr) {
			sprintf_s(cfgName,"\"\"");
		}

		bool ret = true;
		ret &= ExpectOkay(GenCSV(csvName,idx,idxErr,parErr,headErr,sepErr),"Saving %s",csvName);
		ret &= ExpectOkay(WriteConfigFile(cfgName),"Save config for %s",csvName);
		return ret;
	}

	bool LoadConfigWithCsv(NArray<float> &arr)
	{
		LibconfigWrapper lcw;
		lcw.Load(_fileName);

		return lcw.Parse(&arr,"tmp", T_FLOAT, M_NARRAY );
	}

	

	void TestBuildin() {
		TestGroup("Buildin");

		int idx = 0;

		int  rangeMin[5] = {   0, -128,   0,-32768,    0};
		int  rangeMax[5] = {   1,  127, 255, 32767,65535};

		bool fileErr = false;
		bool valErr = false;
		for(int trial=0; trial<50; trial++) {

			if (fileErr || valErr)
				break;
		

			for(int tWrite=(int)T_BOOL; tWrite<=(int)T_DOUBLE; tWrite++) {
				char *wName = ParamManager::ParBase::GetTypeName( (LcwBaseType)tWrite);

				double val = GetNextTestValue(idx,10);
				
				double clippedVal = ClippedVal((LcwBaseType)tWrite, val);
				if ( !ExpectOkay(WriteBuildin((LcwBaseType)tWrite,clippedVal),"Writing temp ascii config") ) {
					fileErr = true;
					break;
				}

				for(int tRead=(int)T_BOOL; tRead<=(int)T_DOUBLE; tRead++) {
					char *rName = ParamManager::ParBase::GetTypeName(  (LcwBaseType)tRead);

					bool expectResult = GetBuildinCompatibility(tWrite, tRead, clippedVal, rangeMin[tRead],rangeMax[tRead]);
					char dummy[16];

					LibconfigWrapper lcw;
					lcw.Load(_fileName);
					bool loadResult = lcw.Parse(&dummy,"tmp", (LcwBaseType)tRead, M_BUILDIN );

					if (expectResult)
						ExpectOkay(loadResult,"O %s->%s %s",wName,rName,_fileContent) ;
					else
						ExpectFail(loadResult,"F %s->%s %s",wName,rName,_fileContent) ;

					if (expectResult != loadResult) {
						valErr = true;
					}
				}
			}
		}

		TestGroup();
	}

	void Test2dCSV() {
		TestGroup("2D CSV special case");

		int idx = 0;	

		for(int test=0; test<10;test++) {

			if (WriteConfigWithCsv("okay",test,idx,false,false,false,false,false,false) ) {
				NArray<float> arr;
				ExpectOkay( LoadConfigWithCsv(arr), "Load NArray");
				if (ExpectOkay( arr.GetDims()==2, "NArray dims == 2") ) {
					bool valErr = false;
					for(unsigned int y=0; y<arr.GetDim(0); y++) {
						for(unsigned int x=0; x<arr.GetDim(1); x++) {
							
							float expVal = (float)x*0.1f + (float)y;
							float hasVal = arr(y,x);

							if (fabs(expVal-hasVal) > 0.00001) {
								ExpectOkay(false,"Narray(%d,%d) value",y,x);
								valErr = true; 
								break;
							}
						}
						if (valErr)
							break;
					}
				}

			}

			
			if (WriteConfigWithCsv("idxE",test,idx,true,false,false,false,false,false) ) {
				NArray<float> arr;
				ExpectFail( LoadConfigWithCsv(arr), "Load NArray idxE");
			}

			if (WriteConfigWithCsv("parE",test,idx,false,true,false,false,false,false) ) {
				NArray<float> arr;
				ExpectFail( LoadConfigWithCsv(arr), "Load NArray parE");
			}

			if (WriteConfigWithCsv("hdrE",test,idx,false,false,true,false,false,false) ) {
				NArray<float> arr;
				ExpectFail( LoadConfigWithCsv(arr), "Load NArray hdrE");
			}
			
			if (WriteConfigWithCsv("sepC",test,idx,false,false,false,true,false,false) ) {
				NArray<float> arr;
				ExpectOkay( LoadConfigWithCsv(arr), "Load NArray sepE");
			}

		}

		if (WriteConfigWithCsv("namE",998,idx,false,false,false,false,true,false) ) {
			NArray<float> arr;
			ExpectFail( LoadConfigWithCsv(arr), "Load NArray namE");
		}
		if (WriteConfigWithCsv("NoName",999,idx,false,false,false,false,false,true) ) {
			NArray<float> arr;
			ExpectFail( LoadConfigWithCsv(arr), "Load NArray NoName");
		}

		TestGroup();
	}


	void TestGetValSpecialCases() {
		TestGroup("GetVal() special cases");

		ExpectOkay(WriteConfigFile("\"a\""),"GetVal(char) as string");
		{
			LibconfigWrapper lcw;
			lcw.Load(_fileName);

			char c;
			ExpectOkay(lcw.Parse(&c,"tmp", T_CHAR, M_BUILDIN ), "Parse char as string");
			ExpectOkay(c=='a',"Parse char as string  value");
		}
		{
			LibconfigWrapper lcw;
			lcw.Load(_fileName);

			unsigned char c;
			ExpectOkay(lcw.Parse(&c,"tmp", T_UCHAR, M_BUILDIN ), "Parse uchar as string");
			ExpectOkay(c=='a',"Parse uchar as string  value");
		}

		ExpectOkay(WriteConfigFile("\"ab\""),"GetVal(char) as bad string");
		{
			LibconfigWrapper lcw;
			lcw.Load(_fileName);

			char c;
			ExpectFail(lcw.Parse(&c,"tmp", T_CHAR, M_BUILDIN ), "Parse char as bad string");
		}
		{
			LibconfigWrapper lcw;
			lcw.Load(_fileName);

			unsigned char c;
			ExpectFail(lcw.Parse(&c,"tmp", T_UCHAR, M_BUILDIN ), "Parse uchar as bad string");
		}



		ExpectOkay(WriteConfigFile("4026531840L"),"GetVal(uint) as TypeInt64");
		{
			LibconfigWrapper lcw;
			lcw.Load(_fileName);

			unsigned int i;
			ExpectOkay(lcw.Parse(&i,"tmp", T_UINT, M_BUILDIN ), "Parse uint as TypeInt64");
			ExpectOkay(i==4026531840,"Parse uint as TypeInt64 value");
		}
		

		ExpectOkay(WriteConfigFile("-1000L"),"GetVal(uint) negative as TypeInt64");
		{
			LibconfigWrapper lcw;
			lcw.Load(_fileName);

			unsigned int i;
			ExpectFail(lcw.Parse(&i,"tmp", T_UINT, M_BUILDIN ), "Parse uint negative as TypeInt64");
		}


		TestGroup();
	}

	void GenNArrayError(char *str, char *name) {
		ExpectOkay(WriteConfigFile(str),"Save NArray %s",name);
		{
			LibconfigWrapper lcw;
			lcw.Load(_fileName);

			NArray<float> arr;
			ExpectFail(lcw.Parse(&arr,"tmp", T_FLOAT, M_NARRAY ), "Parse NArray %s",name);
		}
	}

	void GenNArrayOkay(char *str, int dims, char *name) {
		ExpectOkay(WriteConfigFile(str),"Save NArray %s",name);
		{
			LibconfigWrapper lcw;
			lcw.Load(_fileName);

			NArray<float> arr;
			ExpectOkay(lcw.Parse(&arr,"tmp", T_FLOAT, M_NARRAY ), "Parse NArray %s",name);
			ExpectOkay(arr.GetDims()==dims,"Dims NArray %s",name);
		}
	}

	void TestNArraySpecialCases() {
		TestGroup("NArray special cases");

		ExpectOkay(WriteConfigFile("()"),"Save NArray with empty list");
		{
			LibconfigWrapper lcw;
			lcw.Load(_fileName);

			NArray<float> arr;
			ExpectOkay(lcw.Parse(&arr,"tmp", T_FLOAT, M_NARRAY ), "Parse NArray with empty list");
			ExpectOkay(arr.GetDims()==0,"Value NArray with empty list");
		}
		
		GenNArrayError("( 1,2,() )","with bad list"); // Interpreted as 1D array !
		GenNArrayError("( 1, (2) )","with bad first param");
		GenNArrayError("( (1), 3 )","with bad second param");
		GenNArrayError("( (), (1) )","with bad dimensions A");
		GenNArrayError("( (1,1,1,1,1,1,1,1,1,1,1,1), (1) )","with bad dimensions B");
		GenNArrayError("( (1,0,1), (1) )","with bad dimensions C");
		GenNArrayError("( (1,2,3,-4), (1) )","with bad dimensions D");
		GenNArrayError("( (3),(1,false,3) )","with bad value A");

		
		GenNArrayError("( (3),(1,2) )","with bad too few dim(0) values");
		GenNArrayError("( (2,3),( (1,2,3), (1,2) ) )","with bad too few (1,3) values");

		GenNArrayError("( (1,1,1,1,1,1,1,1,1),  (((((((((false)))))))))  )","9 D, but bad single value");

		{
			LibconfigWrapper lcw;

			NArray<float> arr(2,2);
			arr(0,0) = 1;
			arr(0,1) = 2;
			arr(1,0) = 3;
			arr(1,1) = 4;
			ExpectOkay(lcw.Store(&arr,"tmp", T_FLOAT, M_NARRAY ), "Store NArray 2x2");			
			ExpectOkay(lcw.Save(_fileName),"Save NArray 2x2");
		}
		
		{
			LibconfigWrapper lcw;
			lcw.Load(_fileName);

			NArray<float> arr(2,2);
			ExpectOkay(lcw.Parse(&arr,"tmp", T_FLOAT, M_NARRAY ), "Parse NArray 2x2 with init");			
			ExpectOkay( arr(0,0)==1.0,"Parse NArray 2x2 val 0,0");
			ExpectOkay( arr(0,1)==2.0,"Parse NArray 2x2 val 0,1");
			ExpectOkay( arr(1,0)==3.0,"Parse NArray 2x2 val 1,0");
			ExpectOkay( arr(1,1)==4.0,"Parse NArray 2x2 val 1,1");
		}
		
		{
			LibconfigWrapper lcw;

			NArray<float> arr;
			ExpectOkay(lcw.Store(&arr,"tmp", T_FLOAT, M_NARRAY ), "Store NArray empty");		
			ExpectOkay(lcw.Save(_fileName),"Save NArray empty");			
		}

		{
			LibconfigWrapper lcw;
			lcw.Load(_fileName);

			NArray<float> arr;
			ExpectOkay(lcw.Parse(&arr,"tmp", T_FLOAT, M_NARRAY ), "Parse NArray empty");		
			ExpectOkay(arr.GetDims()==0,"NArray empty dims()");			
		}

		GenNArrayOkay("( (1,1,1,1),  ((((1))))  )",4,"4 D, but single value");
		GenNArrayOkay("( (1,1,1,1,1),  (((((1)))))  )",5,"5 D, but single value");
		GenNArrayOkay("( (1,1,1,1,1,1),  ((((((1))))))  )",6,"6 D, but single value");
		GenNArrayOkay("( (1,1,1,1,1,1,1),  (((((((1)))))))  )",7,"7 D, but single value");
		GenNArrayOkay("( (1,1,1,1,1,1,1,1),  ((((((((1))))))))  )",8,"8 D, but single value");
		GenNArrayOkay("( (1,1,1,1,1,1,1,1,1),  (((((((((1)))))))))  )",9,"9 D, but single value");
		GenNArrayOkay("( (1,1,1,1,1,1,1,1,1,1),  ((((((((((1))))))))))  )",10,"10 D, but single value");

		
		TestGroup();
	}

	void TestConfigSpecialCases() {
		TestGroup("Configs special cases");

		{
			LibconfigWrapper lcw;
			lcw.Load(_fileName);

			int i;
			ExpectFail(lcw.Parse(&i,"tmpX", T_INT, M_BUILDIN ), "Parse with unknown name");
		}

		{
			LibconfigWrapper lcw;
			lcw.Load(_fileName);

			int i;
			ExpectFail(lcw.Parse(&i,"tmp X", T_INT, M_BUILDIN ), "Parse with invalid name A");
		}
		
		{
			LibconfigWrapper lcw;
			lcw.Load(_fileName);

			int i;
			ExpectFail(lcw.Parse(&i,"&$xx", T_INT, M_BUILDIN ), "Parse with invalid name B");
		}		
		{
			LibconfigWrapper lcw;
			lcw.Load(_fileName);

			int i;
			ExpectFail(lcw.Parse(&i,"()", T_INT, M_BUILDIN ), "Parse with invalid name C");
		}
		{
			LibconfigWrapper lcw;
			lcw.Load(_fileName);

			int i;
			ExpectFail(lcw.Parse(&i,"x:x", T_INT, M_BUILDIN ), "Parse with invalid name D");
		}
		
		{
			LibconfigWrapper lcw;
			int i = 0;
			ExpectFail(lcw.Store(&i,"()", T_INT, M_BUILDIN ), "Store with invalid name C");
		}
		{
			LibconfigWrapper lcw;
			int i = 0;
			ExpectFail(lcw.Store(&i,"x:x", T_INT, M_BUILDIN ), "Store with invalid name D");
		}
		
		{
			LibconfigWrapper lcw;
			ExpectFail(lcw.GroupEnd("NoStart"), "GroupEnd with no start");
		}

		{
			LibconfigWrapper lcw;
			ExpectFail(lcw.GroupBeg("NameTooLong0123456789012345678901234567890123456789012345678901234567890123456789"), "GroupStart with too long name");
		}

		{
			LibconfigWrapper lcw;
			ExpectFail(lcw.GroupBeg("x:x"), "GroupStart invalid name");
		}
		
		/*
		{
			LibconfigWrapper lcw;
			lcw.Load(_fileName);

			ExpectOkay(lcw.GroupBeg("NotThere"), "GroupStart bad group");
			int i=0;
			ExpectFail(lcw.Parse(&i,"tmp", T_INT, M_BUILDIN ), "Parse in wrong group");
		}
		*/

		{
			LibconfigWrapper lcw;
			ExpectOkay(lcw.GroupBeg("Level_0"),"GroupBeg level 0");
			ExpectOkay(lcw.GroupBeg("Level_1"),"GroupBeg level 1");
			ExpectOkay(lcw.GroupBeg("Level_2"),"GroupBeg level 2");
			ExpectOkay(lcw.GroupBeg("Level_3"),"GroupBeg level 3");
			ExpectOkay(lcw.GroupBeg("Level_4"),"GroupBeg level 4");
			ExpectOkay(lcw.GroupBeg("Level_5"),"GroupBeg level 5");
			ExpectOkay(lcw.GroupBeg("Level_6"),"GroupBeg level 6");
			ExpectOkay(lcw.GroupBeg("Level_7"),"GroupBeg level 7");
			ExpectOkay(lcw.GroupBeg("Level_8"),"GroupBeg level 8");
			ExpectOkay(lcw.GroupBeg("Level_9"),"GroupBeg level 9");
			ExpectFail(lcw.GroupBeg("Level_10"),"GroupBeg level 10");
		}
		
		{
			LibconfigWrapper lcw;
			lcw.Load(_fileName);

			char dummy[128];
			ExpectFail(lcw.Parse(&dummy,"tmp", (LcwBaseType)1000, M_BUILDIN ),   "Parse Bad Buildin BaseType");	
			ExpectFail(lcw.Parse(&dummy,"tmp", (LcwBaseType)1000, M_ARRAY,10 ),  "Parse Bad Array  BaseType");		
			ExpectFail(lcw.Parse(&dummy,"tmp", (LcwBaseType)1000, M_NARRAY,10 ), "Parse Bad NArray  BaseType");		
		}
		{
			LibconfigWrapper lcw;
			lcw.Load(_fileName);

			char dummy[128];
			ExpectFail(lcw.Parse(&dummy,"tmp", T_CHAR, (LcwBaseMode)1000 ), "Parse bad char BaseMode");	
		}
		{
			LibconfigWrapper lcw;

			char dummy[128];
			ExpectFail(lcw.Store(&dummy,"tmp", (LcwBaseType)1000, M_BUILDIN ),  "Store Bad Buildin BaseType");	
			ExpectFail(lcw.Store(&dummy,"tmp", (LcwBaseType)1000, M_ARRAY,10 ), "Store Bad Array  BaseType");		
			ExpectFail(lcw.Store(&dummy,"tmp", (LcwBaseType)1000, M_NARRAY,10 ),"Store Bad NArray  BaseType");		
		}
		{
			LibconfigWrapper lcw;

			char dummy[128];
			ExpectFail(lcw.Parse(&dummy,"tmp", T_CHAR, (LcwBaseMode)1000 ), "Store bad char BaseMode");	
		}



		{
			LibconfigWrapper lcw;

			unsigned int ui = 0xC0000000;
			ExpectOkay(lcw.Store(&ui,"tmp", T_UINT, M_BUILDIN ), "Store big uint");		
			ExpectOkay(lcw.Save(_fileName),"Save NArray empty");			
		}
		{
			LibconfigWrapper lcw;
			lcw.Load(_fileName);

			unsigned int ui;
			ExpectOkay(lcw.Parse(&ui,"tmp", T_UINT, M_BUILDIN ), "Parse big uint");		
			ExpectOkay(ui==0xC0000000,"Value big uint");			
		}

		{
			LibconfigWrapper lcw;

			unsigned int ui[3];
			
			for(int i=0;i<3;i++)
				ui[i] = 0xC0000000;
			ExpectOkay(lcw.Store(&ui,"tmp", T_UINT, M_ARRAY,3 ), "Store big uint[]");		
			ExpectOkay(lcw.Save(_fileName),"Save NArray empty");			
		}
		{
			LibconfigWrapper lcw;
			lcw.Load(_fileName);

			unsigned int ui[3];
			ExpectOkay(lcw.Parse(&ui,"tmp", T_UINT, M_ARRAY,3 ), "Parse big uint[]");		
			ExpectOkay(ui[0]==0xC0000000,"Value big uint[0]");			
			ExpectOkay(ui[1]==0xC0000000,"Value big uint[1]");			
			ExpectOkay(ui[2]==0xC0000000,"Value big uint[2]");			
		}

		TestGroup();
	}

	void TestConfigOptionalCases() {
		TestGroup("Configs optional cases");

		// Prepare a file with INT A, negative INT B and DOUBLE C
		{
			LibconfigWrapper lcw;

			int idx = 7;
			int a = GetNextIntTestValue(idx,0,100000,13);
			int b = GetNextIntTestValue(idx,-100000,-1,13); // B is negativ
			double c = GetNextTestValue(idx, 13);

			lcw.Store(&a,"A", T_INT, M_BUILDIN );
			lcw.Store(&b,"B", T_INT, M_BUILDIN );
			lcw.Store(&c,"C", T_DOUBLE, M_BUILDIN );	

			lcw.Save(_fileName);			
		}

		// Try to read _OPTIONAL_ parameters INT A, UINT B (fails due to error), INT C(fails due to error), INT D(fails, not present)
		{
			LibconfigWrapper lcw;
			int a;
			unsigned int b;
			int c;
			int d;

			lcw.Load(_fileName);

			ExpectOkay(lcw.Parse(&a,"A", T_INT,  M_BUILDIN, true ), "Parse optional int A");	
			ExpectOkay(lcw.GetLastParamFoundInCfgFile(), "A was in config file");
			ExpectOkay(strlen(lcw.GetLastParamErrorMsg()) == 0, "A has no error message");

			ExpectFail(lcw.Parse(&b,"B", T_UINT, M_BUILDIN, true ), "Parse optional negative INT B as UINT");	
			ExpectOkay(lcw.GetLastParamFoundInCfgFile(), "B was in config file");
			ExpectOkay(strlen(lcw.GetLastParamErrorMsg()) != 0, "B has error message");

			ExpectFail(lcw.Parse(&c,"C", T_INT,  M_BUILDIN, true ), "Parse optional DOUBLE C as INT");		
			ExpectOkay(lcw.GetLastParamFoundInCfgFile(), "C was in config file");
			ExpectOkay(strlen(lcw.GetLastParamErrorMsg()) != 0, "C has error message");

			ExpectFail(lcw.Parse(&d,"D", T_INT,  M_BUILDIN, true ), "Parse optional D (not present in config file)");	
			ExpectFail(lcw.GetLastParamFoundInCfgFile(),"D was not in config file");
			ExpectOkay(strlen(lcw.GetLastParamErrorMsg()) != 0, "D has error message");
		}

		TestGroup();
	}

	void TestArraySpecialCases() {

		ExpectOkay(WriteConfigFile("\"HelloWorldWhatsUp\""),"Save long string");
		{
			LibconfigWrapper lcw;
			lcw.Load(_fileName);

			char test[10];

			ExpectFail(lcw.Parse(test,"tmp", T_CHAR, M_ARRAY,10), "Parse char[] too long");
		}
		
		ExpectOkay(WriteConfigFile("12345"),"Save numbers");
		{
			LibconfigWrapper lcw;
			lcw.Load(_fileName);

			char test[10];

			ExpectFail(lcw.Parse(test,"tmp", T_CHAR, M_ARRAY,10), "Parse char[] but no string");
		}
		{
			LibconfigWrapper lcw;
			lcw.Load(_fileName);

			int test[10];
			ExpectFail(lcw.Parse(test,"tmp", T_INT, M_ARRAY,10), "Parse int[] but no list");
		}


		ExpectOkay(WriteConfigFile("(1,2,3,4,5)"),"Save int[5]");
		{
			LibconfigWrapper lcw;
			lcw.Load(_fileName);

			int test[10];
			ExpectFail(lcw.Parse(test,"tmp", T_INT, M_ARRAY,10), "Parse int[10] but few values");
		}

		ExpectOkay(WriteConfigFile("(1,2,3,false,5)"),"Save int[5] with err");
		{
			LibconfigWrapper lcw;
			lcw.Load(_fileName);

			int test[5];
			ExpectFail(lcw.Parse(test,"tmp", T_INT, M_ARRAY,5), "Parse int[5] with err");
		}

		TestGroup();
	}

	void GenFuncTypeFail(char *str, char *name)
	{
		ExpectOkay(WriteConfigFile(str),"Save Functype %s",name);
		{
			LibconfigWrapper lcw;
			lcw.Load(_fileName);

			FuncType ft;
			ExpectFail(lcw.Parse(&ft,"tmp", T_DOUBLE, M_FUNCTYPE ), "Parse Functype %s",name);
		}
	}

	void GenFuncTypeOkay(char *str, char *name, FuncType ftOkay) {
		ExpectOkay(WriteConfigFile(str),"Save Functype %s",name);
		{
			LibconfigWrapper lcw;
			lcw.Load(_fileName);

			FuncType ft;
			ExpectOkay(lcw.Parse(&ft,"tmp", T_DOUBLE, M_FUNCTYPE ), "Parse Functype %s",name);

			ExpectOkay( ft == ftOkay, "Functype parsed okay");
		}
	}

	void TestFuncType() {
		TestGroup("FuncType Parsing");

		FuncType ftC12, ftLin, ftQuad,ftHyp,ftSig,ftExp,ftDy; 
		ftC12.SetConstant(12);
		ftLin.SetLinear(7.5, 0.75);
		ftQuad.SetQuadratic(-13.13, 3.14, 2.81 );
		ftHyp.SetHyperbolic(9.09, -1.82);
		ftSig.SetSigmoid(0.021, 3.14);
		ftExp.SetExponential(2.81, 3.14);
		ftDy.SetDy(-7.339, 765.31);


		GenFuncTypeOkay("12","constant int", ftC12 );
		GenFuncTypeOkay("12.0","constant float", ftC12);
		GenFuncTypeOkay("( \"Constant\", (12) )","constant float", ftC12);
		GenFuncTypeOkay("( \"Constant\", (12.0) )","constant float", ftC12);
		GenFuncTypeOkay("( \"Linear\", (7.5, 0.75) )","linear", ftLin);
		GenFuncTypeOkay("( \"Quadratic\", (-13.13, 3.14, 2.81) )","quadratic", ftQuad);
		GenFuncTypeOkay("( \"Hyperbolic\", (9.09, -1.82) )","hyperbolic", ftHyp);
		GenFuncTypeOkay("( \"Sigmoid\", (0.021, 3.14) )","sigmoid", ftSig);
		GenFuncTypeOkay("( \"Exponential\", (2.81, 3.14) )","exponential", ftExp);
		GenFuncTypeOkay("( \"Dy\", (-7.339, 765.31) )","dy", ftDy);

		TestGroup();

		TestGroup("FuncType Error Checks");

		GenFuncTypeFail("(1, 2, 3)","with too long list");
		GenFuncTypeFail("(1, 2)","without first string");
		GenFuncTypeFail("( \"name\", 2 )","without second list");
		GenFuncTypeFail("( \"bad\", (1,2) )","with bad func");
		GenFuncTypeFail("( \"Constant\", (1,2) )","const with bad params");
		GenFuncTypeFail("( \"Quadratic\", (1,2) )","quadratic with bad params");
		GenFuncTypeFail("( \"Linear\", (1) )","linear with bad params");
		GenFuncTypeFail("( \"Linear\", (false, false) )","linear with bad paramytpe");

		{
			LibconfigWrapper lcw;

			FuncType ft;
			ExpectFail(lcw.Store(&ft,"tmp", T_DOUBLE, M_FUNCTYPE ), "Store 'NONE' Functype");
		}
		TestGroup();
	}


public:
	UtLibconfigWrapper() : UnitTest("LibconfigWrapper")
	{
		_fileContent[0] = 0;
		strcpy_s(_fileName,"Ut_LcW_Config.cfg");
	}

	void RunTests() {
		TestBuildin();
		Test2dCSV();
		TestGetValSpecialCases();
		TestNArraySpecialCases();
		TestArraySpecialCases();
		TestFuncType();
		TestConfigSpecialCases();
		TestConfigOptionalCases();
	}
};

UtLibconfigWrapper test_libconfigWrapper;  ///< Global instance automatically registers to UnitTestManager