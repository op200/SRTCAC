#include<iostream>
#include<fstream>
#include<string>
#include<iomanip>
#include<Windows.h>
#include<direct.h>

using namespace std;

int main(int CMDinNum,char**CMDinOption){

	//读取输入参数
	if(CMDinNum>1&&(strcmp(CMDinOption[1],"-h")==0||strcmp(CMDinOption[1],"--help")==0)){
		cout<<"SRT Cutting Audio Clips\n"
			<<"usage: SRTCAC.exe [Audio's Pathname] [Out files' Path] [Subtitle's Pathname] [Speaker] [Language] [LogLevel]\n";
		return 0;
	}

	string AudioPathname,OutPath,SubPathname,Speaker,Language,LogLevel;
	//wchar_t OutPath[600];

	if(CMDinNum>7){
		cout<<"Options are too lot.";
		return 1;
	}

	if(CMDinNum>=2)
		AudioPathname=CMDinOption[1];
	else{
		cout<<"Input the audio's pathname:";
		cin>>AudioPathname;
	}

	if(CMDinNum>=3)
		OutPath=CMDinOption[2];
	//swprintf(OutPath,600,L"%hs",CMDinOption[2]);
	else{
	RewriteOutPath:
		//char hOutPath[600];
		cout<<"Input the out files' path:";
		//cin>>hOutPath;
		cin>>OutPath;
		//swprintf(OutPath,600,L"%hs",hOutPath);
	}

	if(CMDinNum>=4)
		SubPathname=CMDinOption[3];
	else{
		cout<<"Input the subtitle's pathname:";
		cin>>SubPathname;
	}

	if(CMDinNum>=5)
		Speaker=CMDinOption[4];
	else{
		cout<<"Speaker:";
		cin>>Speaker;
	}

	if(CMDinNum>=6)
		Language=CMDinOption[5];
	else{
		cout<<"Language:";
		cin>>Language;
	}

	if(CMDinNum==7)
		LogLevel=CMDinOption[6];
	else{
		cout<<"LogLevel(0:null, 1:all):";
		cin>>LogLevel;
	}


	if(_chdir(OutPath.c_str())){
		//wcout
		cout
			<<"The out files' path is error, it's:\n\t"
			<<OutPath<<"\n\tRewrite it? Or exit.\n\tYes / No ?";
		if(system("choice")==1)
			goto RewriteOutPath;
		else return 2;
	}


	//处理为输出
	string Output("ffmpeg -hide_banner -y -i \""+AudioPathname+"\"");

	system(("ffmpeg -hide_banner -y -i \""+SubPathname+"\" temp.srt").c_str());
	SubPathname="temp.srt";
	{
		char fillCh=L'-';string inputStr,subText;
		ifstream subRead(SubPathname);
		ofstream listWrite("output.list");
		cout<<left<<setfill('*')<<setw(140)<<"Reading:"<<endl;
		for(int printNum=1;getline(subRead,inputStr);){
			if(inputStr.find(" --> ")!=string::npos){

				getline(subRead,subText);
				listWrite<<
					to_string(printNum)+".wav|"+
					Speaker+"|"+
					Language+"|"+
					subText+"\n";

				if(fillCh=='+')fillCh='-';else fillCh='+';
				cout<<left<<setfill(fillCh)<<setw(50)<<"\""+inputStr+"\"";
				cout<<printNum<<endl;

				if(printNum%100==0){
					if(LogLevel=="0")Output+=">nul 2>&1";
					cout<<"Encoding..."<<endl;
					system(Output.c_str());
					Output="ffmpeg -hide_banner -y -i \""+AudioPathname+"\"";
				}

				Output.append(
					" -ss "+inputStr.substr(0,8)+"."+inputStr.substr(9,3)+
					" -to "+inputStr.substr(17,8)+"."+inputStr.substr(26,3)+
					" "+to_string(printNum)+".wav"
				);

				printNum++;
			}

		}
		subRead.close();

		if(LogLevel=="0")Output+=">nul 2>&1";
		cout<<"Encoding..."<<endl;
		system(Output.c_str());
		Output="ffmpeg -hide_banner -y -i \""+AudioPathname+"\"";

		cout<<left<<setfill('*')<<setw(160)<<"Read end."<<endl;
	}

}