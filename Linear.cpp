#include <iostream>
#include <cstdlib> 
#include <fstream>
#include <algorithm>
#include <vector>
#include <sstream>
#include <map>
using namespace std;

vector<string> split(const string& s, char delimiter)
{
	vector<string> tokens;
	string token;
	istringstream tokenStream(s);
	while (getline(tokenStream, token, delimiter))
	{
		tokens.push_back(token);
	}
	return tokens;
}

void linear(vector<string> splited)
{
	map<vector<string>, int> labels;//в векторе две вещи:используемое значение и значение break
	vector<vector<string>> labels2;//здесь в каждом векторе :сравниваемое значение, используемое значение, значение break и количество меток
	int counterLab=0 ;
	fstream output;
	output.open("linear.txt",ios_base::out);
	output.clear();
	
	for (int j = 0; j < splited.size(); j++)
	{
		if (splited[j] == "case")
		{
			vector<string> inLabels;
			output << "cmp r0,'" << splited[++j][1] << "'" << endl;
			inLabels.push_back(splited[j][1]+"");
			j += 4;
			vector<string> Lbreak;
			inLabels.push_back(splited[j]);
			Lbreak.push_back(splited[j]);
			if (splited[j + 2] == "break")
			{
				Lbreak.push_back("1");
				inLabels.push_back("1");
			}
			else
			{
				Lbreak.push_back("0");
				inLabels.push_back("0");
			}

			inLabels.push_back(counterLab + "");
			labels2.push_back(inLabels);
			labels.insert(pair<vector<string>, int>(Lbreak, counterLab));
			output << "beq .L" << labels.find(Lbreak)->second << endl;
			
				
			counterLab = labels.size();
		}
		if (splited[j] == "default")
		{
			j += 4;
			vector<string> inLabel;
			inLabel.push_back("1-");
			inLabel.push_back(splited[j]);
			inLabel.push_back("1");
			inLabel.push_back(counterLab + "");
			labels2.push_back(inLabel);
			output << "bne .L" << counterLab << endl;


		}

	}
	output << endl;
	for (int i = 0; i < labels2.size(); i++)
	{
		vector<string> tmp;
		tmp.push_back(labels2[i][1]);
		tmp.push_back(labels2[i][2]);
		map<vector<string>,int>::iterator it;
		
		if (labels.find(tmp) != labels.end())
		{
			it = labels.find(tmp);
			output << ".L" << it->second << endl;
			output << "mov r0," << it->first[0] << endl;
			if (it->first[1] == "1")
				output << "bx lr" << endl;
			labels.erase(it);
		}
	}
	output << ".L" << counterLab << endl;
	output << "mov r0," << labels2[labels2.size() - 1][1] << endl;
	output << "bx lr" << endl;

	output.close();
}
void sort(string **&array, int n, int m)
{
	int i = 0;
	string *buf;
	char swap_cnt = 0;
	while (i < n)
	{
		if (i + 1 != n && array[i][0] > array[i + 1][0])
		{
			buf = array[i];
			array[i] = array[i + 1];
			array[i + 1] = buf;
			swap_cnt = 1;
		}
		i++;
		if (i == n && swap_cnt == 1)
		{
			swap_cnt = 0;
			i = 0;
		}
	}
}
void tree(fstream &output, string **&cTOz, int IndexStart,int IndexEnd, map<vector<string>, int> labels, int labelsBranch,int &totalLabelsBranch)
{
	int middle = (IndexEnd-IndexStart) / 2;
	
	output << "cmp r0,'" << cTOz[middle + IndexStart][0] << "'" << endl;
	output << "beq .L" << labels.find(vector<string>{cTOz[middle + IndexStart][1], cTOz[middle + IndexStart][2]})->second << endl;
	if (IndexStart!=middle+ IndexStart)
	{
		output << "bls .L" << labelsBranch << endl;
		labelsBranch++;
	}
	if (middle+ IndexStart != IndexEnd)
	{
		tree(output, cTOz, middle + IndexStart + 1, IndexEnd, labels, labelsBranch, totalLabelsBranch);
	}
	if (IndexStart != middle + IndexStart)
	{
		output << endl << ".L" << --labelsBranch << endl;
		labelsBranch = totalLabelsBranch;
		tree(output, cTOz, IndexStart, middle + IndexStart - 1, labels, labelsBranch, totalLabelsBranch);
	}

}

void log(vector<string> splited)
{
	string **cTOz; // pares of case and values and break parametr and order index
	int cTOzcounter = 0;
	vector<string> default3;
	map<vector<string>, int> labels;//ключ-присваиваемое значение и break, второй параметр- номер метки
	int counterLab = 0;
	
	for (int i = 0; i < splited.size(); i++)
	{
		if (splited[i] == "case")
			cTOzcounter++;
	}
	cTOz = new string*[cTOzcounter];
	for (int i = 0; i < cTOzcounter; i++)
	{
		cTOz[i] = new string[4];
		cTOz[i][3]= to_string(i);
	}

	for (int i = 0, j=0; j < splited.size();j++)
	{
		if (splited[j] == "case")
		{
			cTOz[i][0] = splited[++j][1];
			j += 4;
			cTOz[i][1] = splited[j];
			cTOz[i][2] = splited[j+2] == "break" ? "1" : "0";
			labels.insert(pair<vector<string>, int>(vector<string>{splited[j], cTOz[i][2]}, counterLab));
			counterLab = labels.size();
			i++;
		}
		if (splited[j] == "default")
		{
			j += 4;
			
			default3.push_back(splited[j]);
			default3.push_back("1");
			counterLab++;
		}
	}

	sort(cTOz, cTOzcounter, 4);

	fstream output;
	output.open("binary.txt", ios_base::out);
	output.clear();
	int t = counterLab;
	tree(output, cTOz,0, cTOzcounter-1, labels, counterLab,t);

	for (auto& item : labels)
	{
		output <<endl<< ".L" << item.second << endl;
		output << "mov r0," << item.first[0]<<endl;
		if (item.first[1] == "1")
			output << "bx lr"<<endl;
		else
		{

			int i = 0;
			while (cTOz[i][1]!=item.first[0] || cTOz[i][2]!=item.first[1])
			{
				i++;
			}
			int it =atoi(&cTOz[i][3][0]) + 1;
			if (it != cTOzcounter)
			{
				i = 0;
				while (atoi(&cTOz[i][3][0]) != it)
				{
					i++;
				}
				output << "b .L" << labels.find(vector<string>{cTOz[i][1], cTOz[i][2]})->second << endl;
			}
			else
			{
				output << "b .L" << counterLab-1 << endl;

			}

		}
	}
	
	output << endl << ".L" <<counterLab-1 << endl;
	output << "mov r0," << default3[0] << endl;
	output << "bx lr";

	output.close();
	for (int i = 0; i < 3; i++)
		delete[] cTOz[i];
}
int main()
{
	int tmp=' ';
	string alltxt="";
	fstream file;
	vector<string> splited;
	file.open("algorythm.txt");
	if (!file)
	{
		std::cout << "problem with opening" << endl;
		system("pause");
		return 0;
	}

	
	while (!file.eof())
	{
		char space;
		space = (char)tmp;
		tmp=file.get();
		if ((char)tmp == '\n' || (char)tmp == '\t' || (tmp==' ' && space==' '))
			continue;
		if ((char)tmp == ')' || (char)tmp == '(')
		{
			alltxt = alltxt + ' ' + (char)tmp + ' ';
			tmp = ' ';
			continue;
		}
		if ((char)tmp == ':' || (char)tmp == ';')
		{
			alltxt = alltxt + ' ' + (char)tmp;
			tmp = ' ';
			continue;
		}
		alltxt += (char)tmp;
	}
	file.close();

	
	splited = split(alltxt, ' ');
	for (vector<string>::iterator it = *&splited.begin(); it != splited.end(); ++it)
		std::cout << *it<<endl;

	std::cout << "1-linear\n2-log" << endl;
	int tmb;
	std::cin >> tmb;
	if (tmb == 1)
		linear(splited);
	else if (tmb == 2)
		log(splited);
	
	std::cout << endl;
	//system("pause"); 
	return 0;
}